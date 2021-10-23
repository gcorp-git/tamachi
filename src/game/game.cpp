#pragma once

#include "../tamachi/tamachi.cpp"


namespace game {

	std::string title = "Game";

	std::vector<std::string> paths_images = {
		"assets/images/pico",
	};

	std::unordered_map<std::string, tamachi::Tile*> tiles;

	void init();
	void start();
	void stop();
	void _on_start();
	void _on_stop();
	void _on_update();
	void _on_frame( double delta );

	void init( HINSTANCE hInstance, LPSTR lpCmdLine ) {
		tamachi::init( title, hInstance, lpCmdLine );

		tamachi::buffer::resize( 128, 72, 4 );
		tamachi::buffer::set_bg_color( 0x00 );

		tamachi::fps::set_interval( 1000 );
		tamachi::fps::on_interval([]( uint64_t value ){
			tamachi::set_title( std::to_string( value ) );
			// std::cout << value << std::endl;
		});

		tamachi::on_start( _on_start );
		tamachi::on_stop( _on_stop );
		tamachi::on_update( _on_update );
		tamachi::on_frame( _on_frame );

		tamachi::images::load( paths_images );

		auto pico = tamachi::images::get( "assets/images/pico" );

		tiles["pico"] = tamachi::tiles::create( pico );
		tiles["test"] = tamachi::tiles::create( pico, 2, 2, 4, 4 );

		tiles["pico"]->place( 10, 10, 2 );
		tiles["test"]->place( 30, 10, 3 );
	}

	void start() {
		// tamachi::screen::enable_fullscreen();
		// tamachi::cursor::hide();

		tamachi::start();
	}

	void stop() {
		tamachi::stop();
	}

	void _on_start() {
		//
	}

	void _on_stop() {
		//
	}

	void _on_update() {
		//
	}

	void _on_frame( double delta ) {
		// tamachi::buffer::clear();

		if ( tamachi::input::is_down( VK_UP ) ) {
			tiles["pico"]->show();
			tiles["test"]->show();

			// tamachi::paint::rect( 0x44FF0000, 0, 0, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 0, true );
			// tamachi::paint::rect( 0x4400FF00, 20, 20, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 1, true );
			// tamachi::paint::rect( 0x440000FF, 40, 40, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 2, true );

			// tamachi::paint::line( 0x88FFFF00, 0, 0, tamachi::buffer::width - 40, tamachi::buffer::height - 40, 3 );
		} else {
			tiles["pico"]->hide();
			tiles["test"]->hide();
		}
	}

}