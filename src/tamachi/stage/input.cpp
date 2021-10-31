#pragma once


namespace tamachi {
	namespace stage {

		const int KEYS_SIZE = 256;

		struct Mouse {
			uint32_t x, y;
		};

		class Input {
		public:
			Mouse* mouse = nullptr;

			bool keys[ KEYS_SIZE ] = {};

			Input() {
				mouse = new Mouse();

				_listeners = new Listeners<int64_t>();
			}
		
			~Input() {
				delete mouse;

				_listeners->clear();
				
				delete _listeners;

				_is_enabled = false;
			}
		
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

			Listener on( std::string event, std::function<void(int64_t)> handler ) {
				return _listeners->on( event, handler );
			}

			void off( Listener listener ) {
				_listeners->off( listener );
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

			void update_window_size( uint32_t width, uint32_t height ) {
				_window_size.width = width;
				_window_size.height = height;
			}

			void update_canvas_size( uint32_t width, uint32_t height ) {
				_canvas_size.width = width;
				_canvas_size.height = height;
			}

		private:
			bool _is_enabled = false;

			Listeners<int64_t>* _listeners = nullptr;

			canvas::RectSize _window_size;
			canvas::RectSize _canvas_size;

			void _process_kb( MSG* message ) {
				auto vk_code = reinterpret_cast<uint32_t>( message->wParam );
				bool is_down = ( message->lParam & ( 1 << 31 ) ) == 0;
				bool is_changed = keys[ vk_code ] != is_down;

				keys[ vk_code ] = is_down;

				if ( is_changed ) _listeners->dispatch( is_down ? "keydown" : "keyup", vk_code );
			}

			void _process_mouse( MSG* message ) {
				auto scale = canvas::get_scale( _window_size, _canvas_size );
				auto pad = canvas::get_paddings( _window_size, _canvas_size, scale );

				auto mx = static_cast<double>( GET_X_LPARAM( message->lParam ) );
				auto my = static_cast<double>( GET_Y_LPARAM( message->lParam ) );

				mouse->x = static_cast<uint32_t>( ( mx - pad.left ) / scale );
				mouse->y = static_cast<uint32_t>( ( my - pad.top ) / scale );

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

		};

	}
}