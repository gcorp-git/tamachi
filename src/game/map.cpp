#pragma once


namespace game {

	class Map : public tamachi::scene::Object {
	public:

		Map( tamachi::stage::Stage* stage ) {
			_stage = stage;

			_create_tiles();

			_test();
		}

		~Map() {
			for ( auto it : _tiles ) delete it.second;

			_tiles.clear();

			for ( auto listener : _listeners ) _stage->input->off( listener );

			_listeners.clear();
		}
		
		void frame( double delta ) {
			//
		}

	private:
		tamachi::stage::Stage* _stage = nullptr;

		std::unordered_map<std::string, tamachi::tile::Tile*> _tiles = {};

		std::vector<tamachi::Listener> _listeners = {};

		void _create_tiles() {
			tamachi::image::Image* image = nullptr;

			// todo: Map class should override box() method

			image = tamachi::image::factory::get( "assets/images/red" );
			_tiles["red"] = tamachi::tile::factory::create( image );
			set( _tiles["red"], 50, 50, 0 );

			image = tamachi::image::factory::get( "assets/images/green" );
			_tiles["green"] = tamachi::tile::factory::create( image );
			set( _tiles["green"], 52, 52, 1 );

			image = tamachi::image::factory::get( "assets/images/blue" );
			_tiles["blue"] = tamachi::tile::factory::create( image );
			set( _tiles["blue"], 54, 54, 2 );
		}

		void _test() {
			auto image = tamachi::image::factory::get( "assets/images/blue" );

			auto grid = new tamachi::grid::Grid( 8, 8 );
			auto from = new tamachi::Point();
			auto to = new tamachi::Point();
			auto line_tiles = new std::vector<tamachi::tile::Tile*>();

			auto on_mousedown = _stage->input->on( "mousedown", [ this, grid, line_tiles, from, to, image ]( auto key ){
				if ( key == VK_RBUTTON ) {
					from->x = _stage->input->mouse->x + _stage->camera->x;
					from->y = _stage->input->mouse->y + _stage->camera->y;
				}

				if ( key == VK_LBUTTON ) {
					to->x = _stage->input->mouse->x + _stage->camera->x;
					to->y = _stage->input->mouse->y + _stage->camera->y;

					for ( auto tile : *line_tiles ) unset( tile );

					line_tiles->clear();

					grid->ray( *from, *to, 48, [ this, grid, line_tiles, image ]( auto cell, auto intersection ){
						auto tile = tamachi::tile::factory::create( image );

						double x = cell.x * grid->get_cell_width();
						double y = cell.y * grid->get_cell_height();
						double z = 4;

						set( tile, x, y, z );

						line_tiles->push_back( tile );

						return false;
					});
				}
			});

			_listeners.push_back( on_mousedown );
		}

	};

}