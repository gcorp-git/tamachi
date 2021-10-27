#pragma once


namespace tamachi {
	namespace stage {

		struct Sprite {
			int64_t x, y, z;
			Tile* tile;
		};

		class Camera {
		public:
			int64_t x = 0;
			int64_t y = 0;
			int64_t z = 0;

			Camera( canvas::Canvas* new_canvas ) {
				_canvas = new_canvas;
				_previous = new std::unordered_map<uint64_t, Sprite>();
			}

			~Camera() {
				_clear();

				delete _previous;
			}

			void set_scene( scene::Scene* new_scene ) {
				_clear();

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

				auto _next = new std::unordered_map<uint64_t, Sprite>();

				std::vector<scene::Sprite*> result = {};

				_scene->box( x, y, z, width, height, _depth, &result );

				for ( auto it : result ) {
					Sprite sprite = {};

					sprite.tile = it->tile;
					sprite.x = it->x - x;
					sprite.y = it->y - y;
					sprite.z = it->z - z;

					_canvas->show( sprite.tile, sprite.x, sprite.y, sprite.z );
				}

				// todo: remove all sprites from previous time that aren't now

				// todo: remember sprites
				
				result.clear();
			}
		
		private:
			canvas::Canvas* _canvas = nullptr;
			scene::Scene* _scene = nullptr;

			uint64_t _depth = 0;

			std::unordered_map<uint64_t, Sprite>* _previous = nullptr;

			void _clear() {
				// todo: hide all previous sprites
			
				_previous->clear();
			}

		};

	}
}