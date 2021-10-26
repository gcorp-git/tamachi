#pragma once

#include "head.cpp"
#include "buffer.cpp"
#include "input.cpp"
#include "factories/images.cpp"
#include "factories/casts.cpp"
#include "factories/tiles.cpp"


namespace tamachi {
	namespace canvas {

		bool _is_inited = false;
		bool _is_attached = false;
		bool _needs_refresh = false;

		std::vector<Tile*> _removed_tiles = {};

		void init();
		void attach( HDC hdc );
		void detach();
		void render( Tile* tile );
		void remove( Tile* tile );
		void frame();
		void flush(
			HDC hdc, stage::FLUSH_MODE mode,
			uint32_t x, uint32_t y, uint32_t width, uint32_t height
		);
		void set_size( uint32_t width, uint32_t height );
		void set_depth( uint32_t depth );
		void set_bg( uint8_t bg );
		void reset();
		void _update();

		void init() {
			if ( _is_inited ) return;

			_is_inited = true;

			buffer::init();

			auto on_update = []( auto nothing ){ _needs_refresh = true; };

			buffer::on( "update", on_update );
			casts::on( "update", on_update );
		}

		void attach( HDC hdc ) {
			if ( !_is_inited ) return;

			buffer::attach( hdc );
		}

		void detach() {
			if ( !_is_inited ) return;

			buffer::detach();
		}

		void render( Tile* tile ) {
			if ( !_is_inited ) return;

			casts::set( tile );

			stage::_is_changed = true;
		}

		void remove( Tile* tile ) {
			if ( !_is_inited ) return;

			tile->visible = false;

			render( tile );

			_removed_tiles.push_back( tile );
		}

		void frame() {
			for ( auto tile : _removed_tiles ) {
				casts::unset( tile );
				tiles::destroy( tile );
			}

			_removed_tiles.clear();
		}

		void flush(
			HDC hdc=NULL, stage::FLUSH_MODE mode=stage::MODE_DEFAULT,
			uint32_t x=0, uint32_t y=0, uint32_t width=0, uint32_t height=0
		) {
			if ( !_is_inited ) return;

			if ( _needs_refresh ) casts::refresh();

			casts::each([]( auto layer ){
				void* memory;
				uint32_t mw, mh;
				uint32_t x, y, z, width, height;
				uint32_t dx, dy, dw, dh;

				// todo: @opt - two loops is too much
				// as an option, "draw" and "clear" casts can be stored in different maps;
				// then it would be two different loops without repeating.

				for ( auto it : *layer ) {
					auto cast = it.second;

					if ( cast->previous.visible ) {
						x = cast->previous.x;
						y = cast->previous.y;
						z = cast->previous.z;

						width = cast->previous.width;
						height = cast->previous.height;

						buffer::clear( x, y, z, width, height );
					}
				}

				for ( auto it : *layer ) {
					auto cast = it.second;

					if ( cast->current.visible ) {
						memory = cast->tile->image->memory;
						
						mw = cast->tile->image->width;
						mh = cast->tile->image->height;

						x = cast->current.x;
						y = cast->current.y;
						z = cast->current.z;

						width = cast->current.width;
						height = cast->current.height;

						dx = cast->current.dx;
						dy = cast->current.dy;
						dw = cast->current.dw;
						dh = cast->current.dh;

						buffer::draw( memory, mw, mh, x, y, z, width, height, dx, dy, dw, dh );
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
					cast->previous.visible = cast->current.visible;
				}
			});

			casts::clear();

			buffer::flush( hdc, mode, x, y, width, height );
		}

		void set_size( uint32_t width, uint32_t height ) {
			buffer::set_size( width, height );
			input::update_canvas_size( width, height );
		}

		void set_depth( uint32_t depth ) {
			buffer::set_depth( depth );
			casts::set_depth( depth );
		}

		void set_bg( uint8_t bg ) {
			buffer::set_bg( bg );
		}

		void reset() {
			if ( !_is_inited ) return;

			buffer::reset();
			casts::reset();
			tiles::reset();

			_needs_refresh = false;
		}

	}
}