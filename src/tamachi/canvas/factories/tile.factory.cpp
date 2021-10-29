#pragma once


namespace tamachi {
	namespace tile {

		struct Tile {
			uint64_t id;

			image::Image* image;

			uint32_t dx;
			uint32_t dy;
			uint32_t dw;
			uint32_t dh;

			uint32_t width;
			uint32_t height;
		};

		namespace factory {

			uint64_t _IDS = 0;

			auto _storage = new Storage<uint64_t, Tile>();

			Tile* create( image::Image* image, uint32_t dx=0, uint32_t dy=0, uint32_t dw=0, uint32_t dh=0, uint32_t width=0, uint32_t height=0 ) {
				if ( !image || !image->memory || !image->width || !image->height ) return nullptr;

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

				_storage->destroy( tile->id );
			}

			void clear() {
				_storage->clear();
			}

		}
	}
}