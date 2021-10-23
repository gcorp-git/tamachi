#pragma once

#include "../head.cpp"
#include "image.cpp"


namespace tamachi {

	uint64_t _TILE_IDS = 0;

	class Tile {
	public:
		uint64_t id = 0;

		Tile( Image* image, uint32_t dx=0, uint32_t dy=0, uint32_t dw=0, uint32_t dh=0 ) {
			id = ++_TILE_IDS;

			if ( !image->is_ok() || dx >= image->width || dy >= image->height ) return;

			if ( !dw ) dw = image->width;
			if ( !dh ) dh = image->height;

			if ( dx + dw > image->width ) dw = image->width - dx;
			if ( dy + dh > image->height ) dh = image->height - dy;

			if ( !dw || !dh ) return;

			_image = image;

			_dx = dx;
			_dy = dy;
			_dw = dw;
			_dh = dh;

			_is_ok = true;
		}

		~Tile() {
			hide();
		}

		void place( uint32_t x, uint32_t y, uint32_t z ) {
			if ( _x == x && _y == y && _z == z ) return;

			if ( _is_visible ) _clear();

			_x = x;
			_y = y;
			_z = z;

			if ( _is_visible ) _render();
		}

		void move( int64_t dx=0, int64_t dy=0, int64_t dz=0 ) {
			place( _x + dx, _y + dy, _z + dz );
		}

		void show() {
			if ( !_is_ok ) return;
			if ( _is_visible ) return;

			if ( _is_visible ) _clear();

			_render();
			
			_is_visible = true;
		}

		void hide() {
			if ( !_is_ok ) return;
			if ( !_is_visible ) return;

			_clear();
			
			_is_visible = false;
		}

		bool is_ok() {
			return _is_ok;
		}

	private:
		bool _is_ok = false;
		bool _is_visible = false;

		Image* _image = NULL;

		uint32_t _dx = 0;
		uint32_t _dy = 0;
		uint32_t _dw = 0;
		uint32_t _dh = 0;

		uint32_t _x = 0;
		uint32_t _y = 0;
		uint32_t _z = 0;

		void _render() {
			auto p = reinterpret_cast<uint32_t*>( _image->bitmap );
			
			uint32_t color;

			for ( uint32_t iy = 0; iy < _dh; ++iy ) {
				for ( uint32_t ix = 0; ix < _dw; ++ix ) {
					color = *( p + ( _dy + iy ) * _image->width + ( _dx + ix ) );

					buffer::set_pixel( color, _x + ix, _y + iy, _z );
				}
			}
		}

		void _clear() {
			for ( uint32_t iy = 0; iy < _dh; ++iy ) {
				for ( uint32_t ix = 0; ix < _dw; ++ix ) {
					buffer::set_pixel( 0, _x + ix, _y + iy, _z );
				}
			}
		}

	};

}