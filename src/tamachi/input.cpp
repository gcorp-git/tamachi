#pragma once


namespace tamachi {
	namespace input {

		const int BUTTONS_COUNT = 0xFF;

		struct BUTTON_STATE {
			bool is_down = false;
			bool is_changed = false;
		};

		BUTTON_STATE buttons[ BUTTONS_COUNT ] = {};

		void reset() {
			for ( int i = 0; i < BUTTONS_COUNT; ++i ) {
				buttons[ i ].is_changed = false;
			}
		}

		inline bool is_down( uint8_t button ) {
			return buttons[ button ].is_down;
		}

		inline bool changed( uint8_t button ) {
			return buttons[ button ].is_changed;
		}

		inline bool pressed( uint8_t button ) {
			return buttons[ button ].is_down && buttons[ button ].is_changed;
		}

		inline bool released( uint8_t button ) {
			return !buttons[ button ].is_down && buttons[ button ].is_changed;
		}

		bool process( MSG* message ) {
			switch ( message->message ) {
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
				case WM_SYSKEYUP:
				case WM_SYSKEYDOWN:
				case WM_KEYUP:
				case WM_KEYDOWN: {
					auto vk_code = reinterpret_cast<uint32_t>( message->wParam );
					bool is_down = ( ( message->lParam & ( 1 << 31 ) ) == 0 );

					if ( buttons[ vk_code ].is_down != is_down ) {
						buttons[ vk_code ].is_changed = true;
					}

					buttons[ vk_code ].is_down = is_down;
				} break;
				default: {
					return false;
				} break;
			}

			if ( buttons[ VK_MENU ].is_down && buttons[ VK_F4 ].is_down ) return false;

			return true;
		}

	}
}