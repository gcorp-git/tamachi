#pragma once


namespace game {

	class Hero : public tamachi::scene::Object {
	public:
		
		Hero( tamachi::stage::Stage* stage ) {
			_stage = stage;

			_create_tiles();
		}

		~Hero() {
			delete _tile;
		}

		void frame( double delta ) {
			_dx = 0;
			_dy = 0;

			if ( _stage->input->keys[ VK_UP ] ) _dy += -1;
			if ( _stage->input->keys[ VK_RIGHT ] ) _dx += +1;
			if ( _stage->input->keys[ VK_DOWN ] ) _dy += +1;
			if ( _stage->input->keys[ VK_LEFT ] ) _dx += -1;

			if ( _dx != 0 && _dy != 0 ) { _dx *= 0.7; _dy *= 0.7; }

			_move( delta );
		}

	private:
		double _speed = 0.6 / 0.016666;

		double _dx = 0;
		double _dy = 0;

		tamachi::stage::Stage* _stage = nullptr;
		
		tamachi::tile::Tile* _tile = nullptr;

		void _create_tiles() {
			auto image = tamachi::image::factory::get( "assets/images/hero" );

			_tile = tamachi::tile::factory::create( image );

			set( _tile, 0, 0, 0 );
		}

		void _move( double delta ) {
			if ( !_dx && !_dy ) return;

			// x += _dx * _speed * delta;
			// y += _dy * _speed * delta;

			_stage->camera->x += _dx * _speed * delta;
			_stage->camera->y += _dy * _speed * delta;
		}

	};

}