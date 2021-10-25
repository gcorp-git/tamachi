#pragma once

#include "head.cpp"


namespace game {
	namespace hero {

		double speed = 0.6 / 0.016666;

		double dx = 0;
		double dy = 0;

		double x = 0;
		double y = 0;
		uint32_t z = 3;

		std::unordered_map<std::string, uint64_t> _listeners = {};

		bool _is_created = false;

		void create();
		void frame( double delta );
		void _move( double delta );

		void create() {
			if ( _is_created ) return;

			auto pico = tamachi::images::get( "assets/images/pico" );
			
			tiles["hero"] = tamachi::tiles::create( pico );
			tiles["hero"]->x = static_cast<uint32_t>( x );
			tiles["hero"]->y = static_cast<uint32_t>( y );
			tiles["hero"]->z = z;
			tiles["hero"]->visible = true;
			tamachi::canvas::render( tiles["hero"] );

			_listeners["mousedown"] = tamachi::input::on( "mousedown", []( int64_t key ){
				if ( key == VK_LBUTTON ) std::cout << "mouse::left down" << std::endl;
			});

			_listeners["mouseup"] = tamachi::input::on( "mouseup", []( int64_t key ){
				if ( key == VK_LBUTTON ) std::cout << "mouse::left up" << std::endl;
			});

			_listeners["mousewheel"] = tamachi::input::on( "mousewheel", []( int64_t delta ){
				if ( delta < 0 ) {
					std::cout << "scroll down " << delta << std::endl;
				} else {
					std::cout << "scroll up " << delta << std::endl;
				}
			});

			auto red = tamachi::images::get( "assets/images/red" );
			auto green = tamachi::images::get( "assets/images/green" );
			auto blue = tamachi::images::get( "assets/images/blue" );

			tiles["red"] = tamachi::tiles::create( red );
			tiles["red"]->x = 50;
			tiles["red"]->y = 50;
			tiles["red"]->z = 0;
			tiles["red"]->visible = true;
			tamachi::canvas::render( tiles["red"] );

			tiles["green"] = tamachi::tiles::create( green );
			tiles["green"]->x = 52;
			tiles["green"]->y = 52;
			tiles["green"]->z = 1;
			tiles["green"]->visible = true;
			tamachi::canvas::render( tiles["green"] );

			tiles["blue"] = tamachi::tiles::create( blue );
			tiles["blue"]->x = 54;
			tiles["blue"]->y = 54;
			tiles["blue"]->z = 2;
			tiles["blue"]->visible = true;
			tamachi::canvas::render( tiles["blue"] );

			_is_created = true;
		}

		void destroy() {
			if ( !_is_created ) return;

			for ( auto it : _listeners ) tamachi::input::off( it.first, it.second );

			dx = 0;
			dy = 0;

			x = 0;
			y = 0;

			_is_created = false;
		}

		void frame( double delta ) {
			if ( !_is_created ) return;

			dx = 0;
			dy = 0;

			if ( tamachi::input::keys[ VK_UP ] ) dy += -1;
			if ( tamachi::input::keys[ VK_RIGHT ] ) dx += +1;
			if ( tamachi::input::keys[ VK_DOWN ] ) dy += +1;
			if ( tamachi::input::keys[ VK_LEFT ] ) dx += -1;

			if ( dx != 0 && dy != 0 ) { dx *= 0.7; dy *= 0.7; }

			_move( delta );
		}

		void _move( double delta ) {
			if ( !dx && !dy ) return;

			x += dx * speed * delta;
			y += dy * speed * delta;

			if ( x < 0 ) x = 0;
			if ( x > WIDTH - 8 ) x = WIDTH - 8;

			if ( y < 0 ) y = 0;
			if ( y > HEIGHT - 8 ) y = HEIGHT - 8;

			tiles["hero"]->x = static_cast<uint32_t>( x );
			tiles["hero"]->y = static_cast<uint32_t>( y );
			tamachi::canvas::render( tiles["hero"] );
		}

	}
}