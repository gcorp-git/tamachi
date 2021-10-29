#pragma once

#include "head.cpp"
#include "factories/image.factory.cpp"
#include "factories/tile.factory.cpp"
#include "factories/cast.factory.cpp"
#include "buffer.cpp"


namespace tamachi {
	namespace canvas {

		class Canvas {
		public:
			
			Canvas() {
				_listeners = new Listeners<bool>();
				_buffer = new Buffer();
				_casts = new cast::CastFactory();
			}

			~Canvas() {
				tile::factory::clear();
			
				delete _listeners;
				delete _buffer;
				delete _casts;
			}

			uint64_t on( std::string event, std::function<void(bool)> listener ) {
				return _listeners->on( event, listener );
			}

			void off( std::string event, uint64_t id ) {
				_listeners->off( event, id );
			}

			bool is_changed() { return _is_changed; }
			
			uint32_t width() { return _width; }
			uint32_t height() { return _height; }
			uint32_t depth() { return _depth; }

			void attach( HDC hdc ) {
				_buffer->attach( hdc );
			}

			void detach() {
				_buffer->detach();
			}

			void show( tile::Tile* tile, int64_t x=0, int64_t y=0, int64_t z=0 ) {
				if ( !tile ) return;

				_casts->show( tile, x, y, z );
			
				_is_changed = true;
			}

			void hide( tile::Tile* tile ) {
				if ( !tile ) return;

				_casts->hide( tile );

				_is_changed = true;
			}

			void remove( tile::Tile* tile ) {
				if ( !tile ) return;

				hide( tile );

				_removed_tiles.push_back( tile );
			}

			void frame( double delta ) {
				for ( auto tile : _removed_tiles ) {
					_casts->unset( tile );

					tile::factory::destroy( tile );
				}

				_removed_tiles.clear();
			}

			void refresh() {
				_is_updated = true;
				_is_changed = true;
			}

			void set_size( uint32_t width, uint32_t height ) {
				_width = width;
				_height = height;

				_buffer->set_size( width, height );

				_listeners->dispatch( "update", false );

				_is_updated = true;
				_is_changed = true;
			}

			void set_depth( uint32_t depth ) {
				_depth = depth;

				_buffer->set_depth( depth );
				_casts->set_depth( depth );
			
				_listeners->dispatch( "update", false );

				_is_updated = true;
				_is_changed = true;
			}

			void set_bg( uint8_t bg=0x00 ) {
				_buffer->set_bg( bg );
			
				_listeners->dispatch( "update", false );

				_is_updated = true;
				_is_changed = true;
			}

			void flush(
				HDC hdc=NULL, FLUSH_MODE mode=MODE_DEFAULT,
				uint32_t x=0, uint32_t y=0, uint32_t width=0, uint32_t height=0
			) {
				if ( _is_updated ) _casts->refresh();

				_casts->each([ this ]( auto layer ){
					_render_layer_casts( layer );
				});

				_casts->clear();

				_buffer->flush( hdc, mode, x, y, width, height );

				_is_changed = false;
				_is_updated = false;
			}

		private:
			bool _is_attached = false;
			bool _is_changed = false;
			bool _is_updated = false;

			uint32_t _width = 0;
			uint32_t _height = 0;
			uint32_t _depth = 0;

			Buffer* _buffer = nullptr;
			cast::CastFactory* _casts = nullptr;

			std::vector<tile::Tile*> _removed_tiles = {};
		
			Listeners<bool>* _listeners = nullptr;
			
			void _render_layer_casts( std::unordered_map<uint64_t, cast::Cast*>* casts ) {
				cast::Cast* cast;
				void* memory;
				uint32_t mw, mh;
				int64_t x, y, z;
				int64_t width, height;
				uint32_t dx, dy, dw, dh;

				for ( auto it : *casts ) {
					cast = it.second;

					if ( cast->previous.is_visible ) {
						x = cast->previous.x;
						y = cast->previous.y;
						z = cast->previous.z;

						width = cast->previous.width;
						height = cast->previous.height;

						_buffer->clear( x, y, z, width, height );
					}
				}

				for ( auto it : *casts ) {
					cast = it.second;

					if ( cast->current.is_visible ) {
						memory = cast->tile.image->memory;
						
						mw = cast->tile.image->width;
						mh = cast->tile.image->height;

						x = cast->current.x;
						y = cast->current.y;
						z = cast->current.z;

						width = cast->current.width;
						height = cast->current.height;

						dx = cast->current.dx;
						dy = cast->current.dy;
						dw = cast->current.dw;
						dh = cast->current.dh;

						_buffer->draw( memory, mw, mh, x, y, z, width, height, dx, dy, dw, dh );
					}

					cast->previous.x = cast->current.x;
					cast->previous.y = cast->current.y;
					cast->previous.z = cast->current.z;
					cast->previous.width = cast->current.width;
					cast->previous.height = cast->current.height;
					cast->previous.dx = cast->current.dx;
					cast->previous.dy = cast->current.dy;
					cast->previous.dw = cast->current.dw;
					cast->previous.dh = cast->current.dh;
					cast->previous.is_visible = cast->current.is_visible;
				}
			}

		};

	}
}