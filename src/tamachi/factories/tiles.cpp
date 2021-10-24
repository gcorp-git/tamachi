#pragma once

#include "../objects/tile.cpp"


namespace tamachi {
	namespace tiles {

		std::unordered_map<uint64_t, Tile*> _storage = {};

		Tile* create( Image* image, uint32_t dx=0, uint32_t dy=0, uint32_t dw=0, uint32_t dh=0 ) {
			if ( !image ) std::cout << "WARNING: tile got NULL instead of image";

			auto tile = new Tile( image, dx, dy, dw, dh );

			_storage.insert_or_assign( tile->id, tile );

			return tile;
		}

		void update() {
			for ( auto it : _storage ) it.second->update();
		}
		
		void reset() {
			for ( auto it : _storage ) delete it.second;

			_storage.clear();
		}

	}
}