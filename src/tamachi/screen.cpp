#pragma once


namespace tamachi {
	namespace screen {

		WINDOWPLACEMENT prev_pos = { sizeof(prev_pos) };

		uint32_t width = 0;
		uint32_t height = 0;

		uint32_t window_width = 0;
		uint32_t window_height = 0;

		bool is_fullscreen = false;

		void _on_resize( uint32_t new_width, uint32_t new_height );

		void enable_fullscreen() {
			if ( is_fullscreen ) return;

			is_fullscreen = true;

			SetWindowLong( window, GWL_STYLE, GetWindowLong( window, GWL_STYLE ) & ~WS_OVERLAPPEDWINDOW );
			
			GetWindowPlacement( window, &prev_pos );

			MONITORINFO mi = { sizeof(mi) };
			
			GetMonitorInfo( MonitorFromWindow( window, MONITOR_DEFAULTTOPRIMARY ), &mi );
			
			width = mi.rcMonitor.right - mi.rcMonitor.left;
			height = mi.rcMonitor.bottom - mi.rcMonitor.top;
			
			SetWindowPos( window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, width, height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
			
			_is_changed = true;
			_is_updated = true;
		}

		void disable_fullscreen() {
			if ( !is_fullscreen ) return;

			is_fullscreen = false;

			SetWindowLong( window, GWL_STYLE, GetWindowLong( window, GWL_STYLE ) | WS_OVERLAPPEDWINDOW );
			SetWindowPlacement( window, &prev_pos );

			width = window_width;
			height = window_height;

			SetWindowPos( window, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED );
			
			_is_changed = true;
			_is_updated = true;
		}

		void toggle_fullscreen() {
			if ( !is_fullscreen ) {
				enable_fullscreen();
			} else {
				disable_fullscreen();
			}
		}

		void resize( uint32_t new_width, uint32_t new_height ) {
			_on_resize( new_width, new_height );

			if ( !is_fullscreen ) SetWindowPos( window, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED );
		}

		void _on_resize( uint32_t new_width, uint32_t new_height ) {
			window_width = new_width;
			window_height = new_height;

			if ( !is_fullscreen ) {
				width = window_width;
				height = window_height;

				_is_changed = true;
				_is_updated = true;
			}
		}

	}
}