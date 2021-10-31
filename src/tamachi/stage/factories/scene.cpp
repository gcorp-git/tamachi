#pragma once


namespace tamachi {
	namespace scene {

		struct Sprite {
			double x, y, z;
			tile::Tile tile;
		};

		uint64_t _IDS = 0;

		class Object {
		public:
			double x = 0;
			double y = 0;
			double z = 0;

			Object() {
				_id = ++_IDS;
			}

			~Object() {
				for ( auto it : _sprites ) delete it.second;

				_sprites.clear();
			}

			uint64_t id() { return _id; }

			void set( tile::Tile* tile, double _x=0, double _y=0, double _z=0 ) {
				if ( !tile ) return;

				auto found = _sprites.find( tile->id );

				Sprite* sprite = nullptr;

				if ( found == _sprites.end() ) {
					sprite = new Sprite();

					sprite->tile = *tile;

					_sprites.insert_or_assign( tile->id, sprite );
				} else {
					sprite = _sprites[ tile->id ];
				}

				sprite->x = _x;
				sprite->y = _y;
				sprite->z = _z;
			}

			void unset( tile::Tile* tile ) {
				if ( !tile ) return;

				auto found = _sprites.find( tile->id );

				if ( found == _sprites.end() ) return;

				delete _sprites[ tile->id ];
				
				_sprites.erase( found );
			}

			virtual void box( double _x, double _y, double _z, double width, double height, double depth, std::vector<Sprite>* result ) {
				if ( !width || !height || !depth || !result ) return;
				if ( !_sprites.size() ) return;

				for ( auto it : _sprites ) {
					auto sprite = it.second;

					auto tw = static_cast<double>( sprite->tile.width );
					auto th = static_cast<double>( sprite->tile.height );

					if ( x + sprite->x + tw < _x || x + sprite->x >= _x + width ) continue;
					if ( y + sprite->y + th < _y || y + sprite->y >= _y + height ) continue;
					if ( z + sprite->z < _z || z + sprite->z >= _z + depth ) continue;

					Sprite _sprite = *sprite;

					_sprite.x = x + sprite->x;
					_sprite.y = y + sprite->y;
					_sprite.z = z + sprite->z;

					result->push_back( _sprite );
				}
			}

		private:
			uint64_t _id = 0;

			std::unordered_map<uint64_t, Sprite*> _sprites = {};

		};

		class Scene {
		public:
			
			Scene( std::string name ) {
				_name = name;
			}

			~Scene() {
				_objects.clear();
			}

			void set( Object* o ) {
				if ( !o ) return;

				_objects.insert_or_assign( o->id(), o );
			}

			void unset( Object* o ) {
				if ( !o ) return;

				auto found = _objects.find( o->id() );

				if ( found == _objects.end() ) return;

				_objects.erase( found );
			}

			void box( double x, double y, double z, double width, double height, double depth, std::vector<Sprite>* result ) {
				for ( auto it : _objects ) {
					it.second->box( x, y, z, width, height, depth, result );
				}
			}

		private:
			std::string _name = "";

			std::unordered_map<uint64_t, Object*> _objects = {};

		};

	}
}