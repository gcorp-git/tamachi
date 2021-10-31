#pragma once

#include "head.cpp"


namespace tamachi {
	namespace stage {

		class Stage {
		public:
			canvas::Canvas* canvas = nullptr;
			Camera* camera = nullptr;
			Input* input = nullptr;
			
			Stage( uint64_t id, std::function<void(uint64_t, HWND)> on_open, std::function<void(uint64_t, HWND)> on_close ) {
				_id = id;
				_on_open = on_open;
				_on_close = on_close;

				canvas = new canvas::Canvas();
				camera = new Camera( canvas );
				input = new Input();

				canvas->on( "update", [ this ]( auto nothing ){
					input->update_canvas_size( canvas->width(), canvas->height() );
					
					_is_updated = true;
				});

				input->on( "keydown", [ this ]( auto vk_code ){
					if ( input->keys[ VK_MENU ] && input->keys[ VK_RETURN ] ) toggle_fullscreen();
				});
			
				_listeners = new Listeners<double>();
			}
			
			~Stage() {
				delete canvas;
				delete camera;
				delete input;

				delete _listeners;
			}

			Listener on( std::string event, std::function<void(double)> handler ) {
				return _listeners->on( event, handler );
			}

			void off( Listener listener ) {
				_listeners->off( listener );
			}

			void open( uint32_t width=0, uint32_t height=0 ) {
				if ( _is_opened ) return;

				_is_opened = true;

				if ( !width ) width = _width;
				if ( !height ) height = _height;

				_create_window();

				_on_open( _id, _window );

				set_size( width, height );

				_hdc = GetDC( _window );

				canvas->attach( _hdc );
				input->enable();

				_listeners->dispatch( "open", 0 );
			}

			uint64_t id() { return _id; }

			void close() {
				if ( !_is_opened ) return;

				_is_opened = false;

				_on_close( _id, _window );

				_listeners->dispatch( "close", 0 );
				_listeners->clear();

				input->disable();
				canvas->detach();

				_destroy_window();
			}

			void set_title( std::string title ) {
				if ( !_is_opened ) return;

				_window_title = title.c_str();

				SetWindowText( _window, _window_title );
			}

			void set_size( uint32_t width, uint32_t height ) {
				if ( !_is_opened ) return;

				_soft_resize( width, height );

				if ( !_is_fullscreen_enabled ) {
					RECT rect = { 0, 0, static_cast<LONG>( _width ), static_cast<LONG>( _height ) };
					AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW );
					SetWindowPos( _window, 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED );
				}
			}

			void enable_fullscreen() {
				if ( !_is_opened ) return;
				if ( _is_fullscreen_enabled ) return;

				_is_fullscreen_enabled = true;

				_enable_fullscreen();

				input->update_window_size( _width, _height );

				_is_updated = true;
			}

			void disable_fullscreen() {
				if ( !_is_opened ) return;
				if ( !_is_fullscreen_enabled ) return;

				_is_fullscreen_enabled = false;

				_disable_fullscreen();

				input->update_window_size( _width, _height );

				_is_updated = true;
			}

			void toggle_fullscreen() {
				!_is_fullscreen_enabled ? enable_fullscreen() : disable_fullscreen();
			}

			void tick() {
				if ( !_is_opened ) return;

				while ( PeekMessage( &_message, _window, 0, 0, PM_REMOVE ) ) {
					if ( !input->process( &_message ) ) {
						TranslateMessage( &_message );
						DispatchMessage( &_message );
					}
				}
			}

			void frame( double delta ) {
				if ( _is_updated ) {
					_listeners->dispatch( "update", delta );
					
					canvas->refresh();
				}

				_listeners->dispatch( "frame", delta );

				camera->frame( delta );

				if ( canvas->is_changed() ) {
					canvas->flush( _hdc, canvas::MODE_CENTER, 0, 0, _width, _height );
				}
				
				canvas->frame( delta );

				fps::frame( delta );
				
				_is_updated = false;
			}

			LRESULT callback( HWND window, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
				LRESULT result = 0;

				switch ( uMsg ) {
					case WM_PAINT: {
						PAINTSTRUCT paint;
						HDC hdc = BeginPaint( _window, &paint );
						canvas->flush( hdc, canvas::MODE_CENTER, 0, 0, _width, _height );
						EndPaint( _window, &paint );
					} break;
					case WM_DISPLAYCHANGE:
					case WM_SIZE: {
						RECT rect;
						GetClientRect( window, &rect );
						_soft_resize( rect.right - rect.left, rect.bottom - rect.top );
					} break;
					case WM_CLOSE: {
						close();
					} break;
					case WM_DESTROY: {
						PostQuitMessage( 0 );
					} break;
					case WM_QUIT: {
						result = wParam;
					} break;
					default: {
						result = DefWindowProc( window, uMsg, wParam, lParam );
					} break;
				}

				return result;
			}

		private:
			uint64_t _id = 0;

			std::function<void(uint64_t, HWND)> _on_open = NULL;
			std::function<void(uint64_t, HWND)> _on_close = NULL;

			HWND _window = NULL;
			HDC _hdc = NULL;
			LPCTSTR _window_title = "";
			WINDOWPLACEMENT _prev_window_pos = { sizeof(_prev_window_pos) };
			MSG _message = {};

			uint32_t _width = 0;
			uint32_t _height = 0;

			uint32_t _windowed_width = 0;
			uint32_t _windowed_height = 0;

			bool _is_opened = false;
			bool _is_fullscreen_enabled = false;

			Listeners<double>* _listeners = nullptr;

			bool _is_updated = false;

			void _create_window() {
				RECT rect = { 0, 0, static_cast<LONG>( _width ), static_cast<LONG>( _height ) };
				
				AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW );
				
				auto rw = rect.right - rect.left;
				auto rh = rect.bottom - rect.top;

				_window = CreateWindow( _wc.lpszClassName, _window_title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rw, rh, 0, 0, _instance, 0 );
				
				if ( !_window ) {
					// todo: CreateWindow error - use GetLastError()
				}
			}

			void _destroy_window() {
				bool ok = DestroyWindow( _window );

				if ( !ok ) {
					// todo: DestroyWindow error - use GetLastError()
				}
			}

			void _enable_fullscreen() {
				SetWindowLong( _window, GWL_STYLE, GetWindowLong( _window, GWL_STYLE ) & ~WS_OVERLAPPEDWINDOW );
				GetWindowPlacement( _window, &_prev_window_pos );

				MONITORINFO mi = { sizeof(mi) };
				
				GetMonitorInfo( MonitorFromWindow( _window, MONITOR_DEFAULTTOPRIMARY ), &mi );
				
				_width = mi.rcMonitor.right - mi.rcMonitor.left;
				_height = mi.rcMonitor.bottom - mi.rcMonitor.top;

				SetWindowPos( _window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, _width, _height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
			}

			void _disable_fullscreen() {
				SetWindowLong( _window, GWL_STYLE, GetWindowLong( _window, GWL_STYLE ) | WS_OVERLAPPEDWINDOW );
				SetWindowPlacement( _window, &_prev_window_pos );

				_width = _windowed_width;
				_height = _windowed_height;

				SetWindowPos( _window, 0, 0, 0, _width, _height, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED );
			}

			void _soft_resize( uint32_t width, uint32_t height ) {
				_windowed_width = width;
				_windowed_height = height;

				if ( !_is_fullscreen_enabled ) {
					_width = _windowed_width;
					_height = _windowed_height;

					input->update_window_size( _width, _height );

					_is_updated = true;
				}
			}

		};

	}
}