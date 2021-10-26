#pragma once

#include "../tamachi/tamachi.cpp"
#include "hero.cpp"


namespace game {

	void init();
	void start();
	void stop();
	void _on_start();
	void _on_stop();
	void _on_frame( double delta );

	void init() {
		tamachi::fps::set_interval( 1.0 );
		tamachi::fps::on_interval([]( auto value ){
			tamachi::stage::rename( "FPS: " + std::to_string( value ) );
		});

		tamachi::on_start( _on_start );
		tamachi::on_stop( _on_stop );
		tamachi::on_frame( _on_frame );

		tamachi::stage::on( "update", []( auto nothing ){
			// todo: on stage update
		});

		tamachi::images::load( paths_images );
	}

	void _on_start() {
		// tamachi::stage::fullscreen::enable();
		// tamachi::cursor::hide();

		tamachi::stage::create( WIDTH, HEIGHT );

		tamachi::canvas::set_size( WIDTH, HEIGHT );
		tamachi::canvas::set_depth( 5 );
		tamachi::canvas::set_bg( 0x00 );
		
		hero::create();
	}

	void _on_stop() {
		tamachi::stage::destroy();
		hero::destroy();
	}

	void _on_frame( double delta ) {
		hero::frame( delta );
	}

}