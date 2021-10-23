#pragma once

#include "../tamachi/tamachi.cpp"


namespace game {

	std::string title = "Game";

	tamachi::Image* pico = NULL;

	void init();
	void start();
	void stop();
	void frame( double delta );

	void init( HINSTANCE hInstance, LPSTR lpCmdLine ) {
		tamachi::init( title, hInstance, lpCmdLine );

		tamachi::buffer::reset( 128, 72, 4 );
		tamachi::buffer::set_bg_color( 0x00 );

		// tamachi::screen::enable_fullscreen();
		// tamachi::cursor::hide();

		tamachi::fps::set_interval( 1000 );
		tamachi::fps::on_interval([]( uint64_t value ){
			tamachi::set_title( std::to_string( value ) );
			// std::cout << value << std::endl;
		});

		tamachi::on_update([](){
			//
		});

		tamachi::on_frame([]( double delta ){
			frame( delta );
		});

		tamachi::on_start([](){
			std::cout << "game started" << std::endl;
		});

		tamachi::on_stop([](){
			std::cout << "game stopped" << std::endl;
		});

		pico = new tamachi::Image( "assets/images/pico" );
	}

	void start() {
		tamachi::start();
	}

	void stop() {
		tamachi::stop();
	}

	void frame( double delta ) {
		tamachi::buffer::clear();

		if ( tamachi::input::is_down( VK_UP ) ) {

			pico->render( 10, 10, 3 );

			// tamachi::paint::fill( 0x44FF0000, 0, 0, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 0 );
			// tamachi::paint::fill( 0x4400FF00, 20, 20, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 1 );
			// tamachi::paint::fill( 0x440000FF, 40, 40, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 2 );

			// tamachi::paint::line( 0x88FFFF00, 0, 0, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 3 );
		}
	}

}