#pragma once


namespace game {
	namespace hero {

		double speed = 0.6 / 0.016666;

		double dx = 0;
		double dy = 0;

		double x = 0;
		double y = 0;
		int64_t z = 3;

		std::unordered_map<std::string, uint64_t> _subscriptions = {};

		bool _is_created = false;

		tamachi::stage::Stage* _stage = nullptr;

		void create( tamachi::stage::Stage* stage );
		void frame( double delta );
		void _move( double delta );

		void create( tamachi::stage::Stage* stage ) {
			if ( _is_created ) return;

			_is_created = true;

			_stage = stage;

			auto scene = tamachi::scene::factory::get( "default" );

			auto pico = tamachi::image::factory::get( "assets/images/pico" );
			
			tiles["hero"] = tamachi::tile::factory::create( pico );
			scene->set( tiles["hero"], static_cast<int64_t>( x ), static_cast<int64_t>( y ), z );

			auto red = tamachi::image::factory::get( "assets/images/red" );
			auto green = tamachi::image::factory::get( "assets/images/green" );
			auto blue = tamachi::image::factory::get( "assets/images/blue" );

			tiles["red"] = tamachi::tile::factory::create( red );
			scene->set( tiles["red"], 50, 50, 0 );

			tiles["green"] = tamachi::tile::factory::create( green );
			scene->set( tiles["green"], 52, 52, 1 );

			tiles["blue"] = tamachi::tile::factory::create( blue );
			scene->set( tiles["blue"], 54, 54, 2 );



			auto grid = new tamachi::grid::Grid( 8, 8 );
			auto from = new tamachi::Point<int64_t>();
			auto to = new tamachi::Point<int64_t>();
			auto line_tiles = new std::vector<tamachi::tile::Tile*>();

			_subscriptions["mousedown"] = _stage->input->on( "mousedown", [ scene, grid, line_tiles, from, to, blue ]( auto key ){
				if ( key == VK_RBUTTON ) {
					from->x = _stage->input->mouse->x + _stage->camera->x;
					from->y = _stage->input->mouse->y + _stage->camera->y;
				}

				if ( key == VK_LBUTTON ) {
					to->x = _stage->input->mouse->x + _stage->camera->x;
					to->y = _stage->input->mouse->y + _stage->camera->y;

					for ( auto tile : *line_tiles ) scene->unset( tile );

					line_tiles->clear();

					grid->ray( *from, *to, 48, [ scene, grid, line_tiles, blue ]( auto cell, auto intersection ){
						auto tile = tamachi::tile::factory::create( blue );

						int64_t x = cell.x * grid->get_cell_width();
						int64_t y = cell.y * grid->get_cell_height();
						int64_t z = 4;

						scene->set( tile, x, y, z );

						line_tiles->push_back( tile );

						return false;
					});
				}
			});



		}

		void destroy() {
			if ( !_is_created ) return;

			for ( auto it : _subscriptions ) _stage->input->off( it.first, it.second );

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

			if ( _stage->input->keys[ VK_UP ] ) dy += -1;
			if ( _stage->input->keys[ VK_RIGHT ] ) dx += +1;
			if ( _stage->input->keys[ VK_DOWN ] ) dy += +1;
			if ( _stage->input->keys[ VK_LEFT ] ) dx += -1;

			if ( dx != 0 && dy != 0 ) { dx *= 0.7; dy *= 0.7; }

			_move( delta );
		}

		void _move( double delta ) {
			if ( !dx && !dy ) return;

			x += dx * speed * delta;
			y += dy * speed * delta;

			_stage->camera->x = static_cast<int64_t>( x );
			_stage->camera->y = static_cast<int64_t>( y );
		}

	}
}