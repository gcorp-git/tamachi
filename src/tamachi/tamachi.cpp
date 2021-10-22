#include "buffer.cpp"
#include "cursor.cpp"
#include "fps.cpp"
#include "head.cpp"
#include "input.cpp"
#include "paint.cpp"
#include "screen.cpp"


namespace tamachi {

	LRESULT CALLBACK callback( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
		LRESULT result = 0;

		switch ( uMsg ) {
			case WM_CLOSE:
			case WM_DESTROY: {
				is_running = false;
			} break;
			case WM_SIZE: {
				RECT rect;

				GetClientRect( hwnd, &rect );
				
				screen::_on_resize( rect.right - rect.left, rect.bottom - rect.top );

				is_changed = true;
			} break;
			case WM_PAINT: {
				PAINTSTRUCT paint;
		
				HDC deviceContext = BeginPaint( window, &paint );

				//

				EndPaint( window, &paint );
			} break;
			default: {
				result = DefWindowProc( hwnd, uMsg, wParam, lParam );
			} break;
		}

		return result;
	}

	void set_title( std::string title ) {
		window_title = title.c_str();

		SetWindowText( window, window_title );
	}

	void init( std::string title, HINSTANCE hInstance, LPSTR lpCmdLine ) {
		window_title = title.c_str();

		WNDCLASS wc = {};
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpszClassName = "TAMACHI_WINDOW_CLASS";
		wc.lpfnWndProc = callback;

		RegisterClass( &wc );

		screen::resize( 320, 240 );

		buffer::init();
		buffer::reset( 320, 240, 1 );

		window = CreateWindow( wc.lpszClassName, window_title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, screen::width, screen::height, 0, 0, hInstance, 0 );
		hdc = GetDC( window );

		bmi = {};
		bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth = buffer::width;
		bmi.bmiHeader.biHeight = -buffer::height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 4 * 8;
		bmi.bmiHeader.biCompression = BI_RGB;
	}

	int start( std::function<void( double )> on_frame ) {
		if ( is_running ) return 0;

		is_running = true;

		_on_frame = on_frame;

		auto last = std::chrono::steady_clock::now();

		while ( is_running ) {
			while ( PeekMessage( &message, window, 0, 0, PM_REMOVE ) ) {
				if ( !input::process( &message ) ) {
					TranslateMessage( &message );
					DispatchMessage( &message );
				}
			}

			if ( input::is_down( VK_MENU ) && input::pressed( VK_RETURN ) ) screen::toggle_fullscreen();

			auto now = std::chrono::steady_clock::now();
			auto delta = std::chrono::duration_cast<std::chrono::microseconds>( now - last );

			if ( delta.count() > 15000 ) {
				last = now;

				if ( _on_frame ) _on_frame( static_cast<double>( delta.count() ) / 1000 );
			}

			if ( is_changed ) {
				bmi.bmiHeader.biWidth = buffer::width;
				bmi.bmiHeader.biHeight = -buffer::height;

				buffer::flush();
			}

			input::reset();
		}

		return 0;
	}

}