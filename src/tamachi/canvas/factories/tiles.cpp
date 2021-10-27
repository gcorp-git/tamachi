#pragma once

#include "tile.cpp"


namespace tamachi {
	namespace tiles {

		uint64_t _IDS = 0;

		auto _storage = new Storage<uint64_t, Tile>();

		Tile* create( Image* image, uint32_t dx=0, uint32_t dy=0, uint32_t dw=0, uint32_t dh=0, uint32_t width=0, uint32_t height=0 ) {
			if ( !image || !image->ok ) return nullptr;

			if ( !dw ) dw = image->width;
			if ( !dh ) dh = image->height;

			if ( !width ) width = image->width;
			if ( !height ) height = image->height;

			auto tile = _storage->create();

			tile->id = ++_IDS;
			tile->image = image;
			tile->dx = dx;
			tile->dy = dy;
			tile->dw = dw;
			tile->dh = dh;
			tile->width = width;
			tile->height = height;

			_storage->set( tile->id, tile );

			return tile;
		}

		void destroy( Tile* tile ) {
			if ( !tile ) return;

			auto id = tile->id;

			tile->image = nullptr;
			tile->x = 0;
			tile->y = 0;
			tile->z = 0;
			tile->visible = false;

			_storage->destroy( id );
		}

		void reset() {
			_storage->clear();
		}

	}

}