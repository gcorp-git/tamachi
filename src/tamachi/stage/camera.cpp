#pragma once


namespace tamachi {
	namespace stage {

		class Camera {
		public:
			int64_t x = 0;
			int64_t y = 0;
			int64_t z = 0;

			Camera( canvas::Canvas* new_canvas ) {
				_canvas = new_canvas;
				_previous = new std::unordered_map<uint64_t, scene::Sprite>();
			}

			~Camera() {
				_hide_previous();

				_previous->clear();

				delete _previous;
			}

			void set_scene( scene::Scene* new_scene ) {
				_hide_previous();

				_previous->clear();

				_scene = new_scene;
			}

			void set_depth( uint64_t depth ) {
				_depth = depth;
			}

			void frame( double delta ) {
				if ( !_canvas || !_scene ) return;

				auto width = _canvas->width();
				auto height = _canvas->height();

				if ( !_depth ) _depth = _canvas->depth();

				auto _next = new std::unordered_map<uint64_t, scene::Sprite>();

				std::vector<scene::Sprite*> result;

				_scene->box( x, y, z, width, height, _depth, &result );

				for ( auto _sprite : result ) {
					scene::Sprite sprite = {};

					sprite.tile = _sprite->tile;
					sprite.x = _sprite->x - x;
					sprite.y = _sprite->y - y;
					sprite.z = _sprite->z - z;

					auto found = _previous->find( sprite.tile.id );

					if ( found != _previous->end() ) _previous->erase( found );

					_next->insert_or_assign( sprite.tile.id, sprite );
				}

				result.clear();

				_hide_previous();

				_previous->clear();

				delete _previous;

				_previous = _next;

				_show_previous();
			}
		
		private:
			canvas::Canvas* _canvas = nullptr;
			scene::Scene* _scene = nullptr;

			uint64_t _depth = 0;

			std::unordered_map<uint64_t, scene::Sprite>* _previous = nullptr;

			void _hide_previous() {
				for ( auto it : *_previous ) {
					auto sprite = it.second;

					_canvas->remove( &sprite.tile );
				}
			}

			void _show_previous() {
				for ( auto it : *_previous ) {
					auto sprite = it.second;

					_canvas->show( &sprite.tile, sprite.x, sprite.y, sprite.z );
				}
			}

		};

	}
}