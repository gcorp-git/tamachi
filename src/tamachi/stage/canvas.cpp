#pragma once

#include "head.cpp"
#include "buffer.cpp"
#include "../factories/casts.cpp"
#include "../factories/tiles.cpp"


namespace tamachi {
	namespace canvas {

		bool _is_inited = false;
		bool _is_attached = false;
		bool _needs_refresh = false;

		void init();
		void attach( HDC hdc );
		void detach();
		void render( Tile* tile );
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

			auto on_update = []( bool nothing ){ _needs_refresh = true; };

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

		void flush(
			HDC hdc=NULL, stage::FLUSH_MODE mode=stage::MODE_DEFAULT,
			uint32_t x=0, uint32_t y=0, uint32_t width=0, uint32_t height=0
		) {
			if ( !_is_inited ) return;

			if ( _needs_refresh ) casts::refresh();

			casts::each([]( std::unordered_map<uint64_t, Cast*>* layer ){
				for ( auto it : *layer ) {
					auto cast = it.second;

					uint32_t _x, _y, _z, _width, _height;

					if ( cast->previous.visible ) {
						_x = cast->previous.x;
						_y = cast->previous.y;
						_z = cast->previous.z;

						_width = cast->previous.width;
						_height = cast->previous.height;

						buffer::clear( _x, _y, _z, _width, _height );
					}

					if ( cast->current.visible ) {
						auto memory = cast->tile->image->memory;
						auto mw = cast->tile->image->width;
						auto mh = cast->tile->image->height;

						_x = cast->current.x;
						_y = cast->current.y;
						_z = cast->current.z;

						_width = cast->current.width;
						_height = cast->current.height;

						auto dx = cast->current.dx;
						auto dy = cast->current.dy;
						auto dw = cast->current.dw;
						auto dh = cast->current.dh;

						buffer::draw( memory, mw, mh, _x, _y, _z, _width, _height, dx, dy, dw, dh );
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

			_needs_refresh = false;
		}

	}
}