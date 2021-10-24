#pragma once

#include "../head.cpp"
#include "buffer.cpp"
#include "input.cpp"


namespace tamachi {
	namespace stage {

		HWND _window;
		HDC _hdc;

		uint32_t _width = 0;
		uint32_t _height = 0;

		uint32_t _windowed_width = 0;
		uint32_t _windowed_height = 0;

		LPCTSTR _window_title = "Game Title";

		WNDCLASS _wc;
		WINDOWPLACEMENT _prev_window_pos = { sizeof(_prev_window_pos) };
		MSG _message;

		bool _is_inited = false;
		bool _is_created = false;

		std::function<void()> _on_close = nullptr;

		void _soft_resize( uint32_t new_width, uint32_t new_height );
		LRESULT CALLBACK _callback( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		namespace fullscreen {

			bool _is_enabled = false;

			bool is_enabled() { return _is_enabled; }

			void enable() {
				if ( !_is_inited ) return;
				if ( _is_enabled ) return;

				_is_enabled = true;

				SetWindowLong( _window, GWL_STYLE, GetWindowLong( _window, GWL_STYLE ) & ~WS_OVERLAPPEDWINDOW );
				GetWindowPlacement( _window, &_prev_window_pos );

				MONITORINFO mi = { sizeof(mi) };
				
				GetMonitorInfo( MonitorFromWindow( _window, MONITOR_DEFAULTTOPRIMARY ), &mi );
				
				_width = mi.rcMonitor.right - mi.rcMonitor.left;
				_height = mi.rcMonitor.bottom - mi.rcMonitor.top;
				
				SetWindowPos( _window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, _width, _height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
			
				_is_changed = true;
				_is_updated = true;
			}

			void disable() {
				if ( !_is_inited ) return;
				if ( !_is_enabled ) return;

				_is_enabled = false;

				SetWindowLong( _window, GWL_STYLE, GetWindowLong( _window, GWL_STYLE ) | WS_OVERLAPPEDWINDOW );
				SetWindowPlacement( _window, &_prev_window_pos );

				_width = _windowed_width;
				_height = _windowed_height;

				SetWindowPos( _window, 0, 0, 0, _width, _height, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED );
			
				_is_changed = true;
				_is_updated = true;
			}

			void toggle() {
				!_is_enabled ? enable() : disable();
			}

		}

		void init( std::function<void()> on_close ) {
			if ( _is_inited ) return;

			_is_inited = true;

			_on_close = on_close;

			_wc = {};
			_wc.style = CS_HREDRAW | CS_VREDRAW;
			_wc.lpszClassName = "TAMACHI_WINDOW_CLASS";
			_wc.lpfnWndProc = _callback;

			RegisterClass( &_wc );

			buffer::init();
		}

		void create( uint32_t new_width, uint32_t new_height ) {
			if ( !_is_inited ) return;
			if ( _is_created ) return;

			_is_created = true;

			_soft_resize( new_width, new_height );

			RECT rect = { 0, 0, static_cast<LONG>( _width ), static_cast<LONG>( _height ) };
			
			AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW );
			
			auto rw = rect.right - rect.left;
			auto rh = rect.bottom - rect.top;

			_window = CreateWindow( _wc.lpszClassName, _window_title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rw, rh, 0, 0, _hInstance, 0 );
			
			if ( !_window ) {
				// todo: CreateWindow error - use GetLastError()
			}

			_hdc = GetDC( _window );

			buffer::create( _window, _hdc );
			input::enable();
		}

		void destroy() {
			if ( !_is_inited ) return;
			if ( !_is_created ) return;

			_is_created = false;

			buffer::destroy();
			input::disable();

			bool ok = DestroyWindow( _window );

			if ( !ok ) {
				// todo: DestroyWindow error - use GetLastError()
			}
		}

		bool is_created() { return _is_created; }

		void tick() {
			if ( !_is_created ) return;

			while ( PeekMessage( &_message, _window, 0, 0, PM_REMOVE ) ) {
				if ( !input::process( &_message ) ) {
					TranslateMessage( &_message );
					DispatchMessage( &_message );
				}
			}
		}

		void frame() {
			if ( !_is_created ) return;
			if ( !_is_changed ) return;

			buffer::render( _width, _height );
		}

		void rename( std::string title ) {
			_window_title = title.c_str();

			if ( _is_created ) SetWindowText( _window, _window_title );
		}

		void resize( uint32_t new_width, uint32_t new_height ) {
			if ( !_is_inited ) return;

			_soft_resize( new_width, new_height );

			if ( !fullscreen::_is_enabled ) {
				RECT rect = { 0, 0, static_cast<LONG>( _width ), static_cast<LONG>( _height ) };
				AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW );
				SetWindowPos( _window, 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED );
			}
		}

		uint32_t get_width() { return _width; }
		uint32_t get_height() { return _height; }

		void reset() {
			if ( !_is_inited ) return;

			if ( is_created ) stage::destroy();

			buffer::reset();
			input::reset();
		}

		void _soft_resize( uint32_t new_width, uint32_t new_height ) {
			if ( !_is_inited ) return;

			_windowed_width = new_width;
			_windowed_height = new_height;

			if ( !fullscreen::_is_enabled ) {
				_width = _windowed_width;
				_height = _windowed_height;

				_is_changed = true;
				_is_updated = true;
			}
		}

		LRESULT CALLBACK _callback( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
			LRESULT result = 0;

			switch ( uMsg ) {
				case WM_PAINT: {
					PAINTSTRUCT paint;
			
					HDC deviceContext = BeginPaint( _window, &paint );

					// todo: WM_PAINT

					EndPaint( _window, &paint );
				} break;
				case WM_DISPLAYCHANGE:
				case WM_SIZE: {
					RECT rect;
					GetClientRect( hwnd, &rect );
					_soft_resize( rect.right - rect.left, rect.bottom - rect.top );
				} break;
				case WM_CLOSE: {
					if ( _on_close ) _on_close();
				} break;
				case WM_DESTROY: {
					PostQuitMessage( 0 );
				} break;
				case WM_QUIT: {
					result = wParam;
				} break;
				default: {
					result = DefWindowProc( hwnd, uMsg, wParam, lParam );
				} break;
			}

			return result;
		}

	}
}