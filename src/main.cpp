#include "tamachi/tamachi.cpp"


int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) {
	std::string title = "Game";

	tamachi::init( title, hInstance, lpCmdLine );
	// tamachi::screen::enable_fullscreen();
	// tamachi::cursor::hide();

	tamachi::buffer::reset( 128, 72, 4 );
	tamachi::buffer::set_bg_color( 0x00 );

	tamachi::fps::start( 1000, [ title ]( uint64_t value ){
		tamachi::set_title( title + " ( " + std::to_string( value ) + " )" );
		// std::cout << value << std::endl;
	});

	auto img = tamachi::Image( "assets/images/pico" );

	return tamachi::start([ &img ]( double delta ){
		tamachi::buffer::clear();

		if ( tamachi::input::is_down( VK_UP ) ) {
			
			img.render( 10, 10, 3 );

			// tamachi::paint::fill( 0x44FF0000, 0, 0, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 0 );
			// tamachi::paint::fill( 0x4400FF00, 20, 20, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 1 );
			// tamachi::paint::fill( 0x440000FF, 40, 40, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 2 );

			// tamachi::paint::line( 0x88FFFF00, 0, 0, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 3 );
		}

		tamachi::fps::frame();
	});
}