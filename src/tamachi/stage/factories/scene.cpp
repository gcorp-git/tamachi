#pragma once


namespace tamachi {
	namespace scene {

		struct Sprite {
			int64_t x, y, z;
			tile::Tile tile;
		};

		class Scene {
		public:
			
			Scene( std::string name ) {
				_name = name;
				_storage = new Storage<uint64_t, Sprite>();
			}

			~Scene() {
				_storage->clear();

				delete _storage;
			}

			void set( tile::Tile* tile, int64_t x=0, int64_t y=0, int64_t z=0 ) {
				if ( !tile ) return;

				auto id = tile->id;
				auto sprite = _storage->get( id );

				if ( !sprite ) sprite = new Sprite();

				sprite->x = x;
				sprite->y = y;
				sprite->z = z;
				sprite->tile = *tile;

				_storage->set( id, sprite );
			}

			void unset( tile::Tile* tile ) {
				if ( !tile ) return;

				auto id = tile->id;
				auto sprite = _storage->get( id );

				if ( !sprite ) return;

				_storage->remove( id );

				delete sprite;
			}

			void box( int64_t x, int64_t y, int64_t z, uint64_t width, uint64_t height, uint64_t depth, std::vector<Sprite*>* result ) {
				if ( !width || !height || !result ) return;

				auto all_sprites = _storage->all();

				if ( !all_sprites->size() ) return;

				auto iw = static_cast<int64_t>( width );
				auto ih = static_cast<int64_t>( height );

				for ( auto it : *all_sprites ) {
					auto sprite = it.second;

					if ( sprite->x + static_cast<int64_t>( sprite->tile.width ) < x ) continue;
					if ( sprite->x >= x + iw ) continue;

					if ( sprite->y + static_cast<int64_t>( sprite->tile.height ) < y ) continue;
					if ( sprite->y >= y + ih ) continue;
					
					if ( sprite->z < z || sprite->z >= z + depth ) continue;

					result->push_back( sprite );
				}
			}

		private:
			std::string _name = "";

			Storage<uint64_t, Sprite>* _storage = nullptr;

		};

	}
}