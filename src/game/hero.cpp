#pragma once

#include "head.cpp"


namespace game {
	namespace hero {

		double speed = 0.6 / 0.016666;

		double dx = 0;
		double dy = 0;

		double x = 0;
		double y = 0;
		uint32_t z = 2;

		std::unordered_map<std::string, uint64_t> _listeners = {};

		bool _is_created = false;

		void create();
		void frame( double delta );
		void _move( double delta );

		void create() {
			if ( _is_created ) return;

			auto pico = tamachi::images::get( "assets/images/pico" );
			
			tiles["hero"] = tamachi::tiles::create( pico );
			tiles["hero"]->place( 0, 0, z );
			tiles["hero"]->show();

			_listeners["mousedown"] = tamachi::input::on( "mousedown", []( uint32_t key ){
				if ( key == VK_LBUTTON ) std::cout << "mouse::left down" << std::endl;
			});

			_listeners["mouseup"] = tamachi::input::on( "mouseup", []( uint32_t key ){
				if ( key == VK_LBUTTON ) std::cout << "mouse::left up" << std::endl;
			});

			_is_created = true;
		}

		void destroy() {
			if ( !_is_created ) return;

			for ( auto it : _listeners ) tamachi::input::off( it.first, it.second );

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

			int32_t nx = static_cast<int32_t>( x );
			int32_t ny = static_cast<int32_t>( y );

			tiles["hero"]->place( nx, ny, z );
		}

	}
}