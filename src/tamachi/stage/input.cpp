#pragma once

#include "../utils/listeners.cpp"


namespace tamachi {
	namespace input {

		namespace mouse {
			uint32_t x = 0;
			uint32_t y = 0;
		}

		const int KEYS_SIZE = 256;
		bool keys[ KEYS_SIZE ] = {};

		auto _listeners = new Listeners<int64_t>();

		bool _is_enabled = false;

		void enable();
		void disable();
		uint64_t on( std::string event, std::function<void(int64_t)> listener );
		void off( std::string event, uint64_t id );
		bool process( MSG* message );
		void reset();
		void _process_kb( MSG* message );
		void _process_mouse( MSG* message );

		void enable() {
			if ( _is_enabled ) return;

			_is_enabled = true;

			//
		}

		void disable() {
			if ( !_is_enabled ) return;

			_is_enabled = false;

			//
		}

		uint64_t on( std::string event, std::function<void(int64_t)> listener ) {
			return _listeners->on( event, listener );
		}

		void off( std::string event, uint64_t id ) {
			_listeners->off( event, id );
		}

		bool process( MSG* message ) {
			if ( !_is_enabled ) return false;

			switch ( message->message ) {
				case WM_SYSKEYUP:
				case WM_SYSKEYDOWN:
				case WM_KEYUP:
				case WM_KEYDOWN: {
					_process_kb( message );
				} break;
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
				case WM_MOUSEMOVE: {
					_process_mouse( message );
				} break;
				case WM_MOUSEWHEEL: {
					auto delta = GET_WHEEL_DELTA_WPARAM( message->wParam );

					_listeners->dispatch( "mousewheel", delta / WHEEL_DELTA );
				} break;
				default: {
					return false;
				} break;
			}

			if ( keys[ VK_MENU ] && keys[ VK_F4 ] ) return false;

			return true;
		}

		void reset() {
			if ( !_is_enabled ) return;

			for ( int i = 0; i < KEYS_SIZE; ++i ) keys[ i ] = false;

			_listeners->clear();

			_is_enabled = false;
		}

		void _process_kb( MSG* message ) {
			auto vk_code = reinterpret_cast<uint32_t>( message->wParam );
			bool is_down = ( message->lParam & ( 1 << 31 ) ) == 0;
			bool is_changed = keys[ vk_code ] != is_down;

			keys[ vk_code ] = is_down;

			if ( is_changed ) _listeners->dispatch( is_down ? "keydown" : "keyup", vk_code );
		}

		void _process_mouse( MSG* message ) {
			mouse::x = static_cast<uint32_t>( GET_X_LPARAM( message->lParam ) );
			mouse::y = static_cast<uint32_t>( GET_Y_LPARAM( message->lParam ) );

			if ( message->message == WM_MOUSEMOVE ) _listeners->dispatch( "mousemove", 0 );

			uint32_t vk_code;
			bool is_down;
			bool is_changed;

			vk_code = VK_LBUTTON;
			is_down = ( message->wParam & MK_LBUTTON ) != 0;
			is_changed = keys[ vk_code ] != is_down;
			keys[ vk_code ] = is_down;

			if ( is_changed ) _listeners->dispatch( is_down ? "mousedown" : "mouseup", vk_code );

			vk_code = VK_MBUTTON;
			is_down = ( message->wParam & MK_MBUTTON ) != 0;
			is_changed = keys[ vk_code ] != is_down;
			keys[ vk_code ] = is_down;

			if ( is_changed ) _listeners->dispatch( is_down ? "mousedown" : "mouseup", vk_code );
			
			vk_code = VK_RBUTTON;
			is_down = ( message->wParam & MK_RBUTTON ) != 0;
			is_changed = keys[ vk_code ] != is_down;
			keys[ vk_code ] = is_down;

			if ( is_changed ) _listeners->dispatch( is_down ? "mousedown" : "mouseup", vk_code );
		}

	}
}