#pragma once

#include "objects/tile.cpp"


namespace tamachi {
	namespace tiles {

		std::unordered_map<uint64_t, Tile*> _storage = {};

		Tile* create( Image* image, uint32_t dx=0, uint32_t dy=0, uint32_t dw=0, uint32_t dh=0 ) {
			auto tile = new Tile( image, dx, dy, dw, dh );

			_storage.insert_or_assign( tile->id, tile );

			return tile;
		}
		
		void reset() {
			for ( auto it : _storage ) delete it.second;

			_storage.clear();
		}

	}
}