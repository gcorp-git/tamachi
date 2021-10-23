#include "buffer.cpp"
#include "cursor.cpp"
#include "fps.cpp"
#include "head.cpp"
#include "images.cpp"
#include "input.cpp"
#include "paint.cpp"
#include "screen.cpp"
#include "tiles.cpp"


namespace tamachi {

	void init( std::string title, HINSTANCE hInstance, LPSTR lpCmdLine );
	void start();
	void stop();
	void set_title( std::string title );
	void on_start( std::function<void()> handler );
	void on_stop( std::function<void()> handler );
	void on_update( std::function<void()> handler );
	void on_frame( std::function<void( double )> handler );
	LRESULT CALLBACK callback( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void init( std::string title, HINSTANCE hInstance, LPSTR lpCmdLine ) {
		if ( _is_inited ) return;

		_is_inited = true;

		window_title = title.c_str();

		WNDCLASS wc = {};
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpszClassName = "TAMACHI_WINDOW_CLASS";
		wc.lpfnWndProc = callback;

		RegisterClass( &wc );

		screen::resize( 320, 240 );
		buffer::init();

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

	void start() {
		if ( _is_running ) return;

		_is_running = true;

		if ( _on_start ) _on_start();

		auto last = std::chrono::steady_clock::now();

		fps::start();

		while ( _is_running ) {
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

				if ( _is_updated && _on_update ) _on_update();
				if ( _on_frame ) _on_frame( static_cast<double>( delta.count() ) / 1000 );
			
				_is_updated = false;

				fps::frame();
			}

			if ( _is_changed ) {
				bmi.bmiHeader.biWidth = buffer::width;
				bmi.bmiHeader.biHeight = -buffer::height;

				buffer::flush();
			}

			input::refresh();
		}
	}

	void stop() {
		if ( !_is_running ) return;

		_is_running = false;

		if ( _on_stop ) _on_stop();

		buffer::reset();
		cursor::reset();
		images::reset();
		input::reset();
		screen::reset();
		tiles::reset();
	}

	void set_title( std::string title ) {
		window_title = title.c_str();

		SetWindowText( window, window_title );
	}

	void on_start( std::function<void()> handler ) {
		_on_start = handler;
	}

	void on_stop( std::function<void()> handler ) {
		_on_stop = handler;
	}

	void on_update( std::function<void()> handler ) {
		_on_update = handler;
	}

	void on_frame( std::function<void( double )> handler ) {
		_on_frame = handler;
	}

	LRESULT CALLBACK callback( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
		LRESULT result = 0;

		switch ( uMsg ) {
			case WM_CLOSE:
			case WM_DESTROY: {
				stop();
			} break;
			case WM_SIZE: {
				RECT rect;

				GetClientRect( hwnd, &rect );

				screen::_on_resize( rect.right - rect.left, rect.bottom - rect.top );
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

}