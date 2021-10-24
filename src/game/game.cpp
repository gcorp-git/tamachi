#pragma once

#include "../tamachi/tamachi.cpp"
#include "hero.cpp"


namespace game {

	void init();
	void start();
	void stop();
	void _on_start();
	void _on_stop();
	void _on_update();
	void _on_frame( double delta );

	void init() {
		tamachi::stage::rename( title );

		tamachi::fps::set_interval( 1.0 );
		tamachi::fps::on_interval([]( uint64_t value ){
			tamachi::stage::rename( std::to_string( value ) );
			// std::cout << value << std::endl;
		});

		tamachi::on_start( _on_start );
		tamachi::on_stop( _on_stop );
		tamachi::on_update( _on_update );
		tamachi::on_frame( _on_frame );

		tamachi::images::load( paths_images );
	}

	void _on_start() {
		// tamachi::stage::fullscreen::enable();
		// tamachi::cursor::hide();

		tamachi::stage::resize( WIDTH, HEIGHT );
		tamachi::buffer::resize( WIDTH, HEIGHT, 4 );
		tamachi::buffer::set_bg_color( 0x00 );

		hero::create();
	}

	void _on_stop() {
		hero::destroy();
	}

	void _on_update() {
		//
	}

	void _on_frame( double delta ) {
		hero::frame( delta );
	}

}