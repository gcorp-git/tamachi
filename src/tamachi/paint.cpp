#pragma once

#include "head.cpp"
#include "stage/buffer.cpp"
#include "objects/image.cpp"


namespace tamachi {
	namespace paint {

		void dot( uint32_t color, uint32_t x, uint32_t y, uint32_t z ) {
			buffer::set_pixel( color, x, y, z );
		}

		void rect( uint32_t color, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t z, bool fill=false ) {
			auto buffer_width = buffer::get_width();
			auto buffer_height = buffer::get_height();

			if ( x + w > buffer_width ) w = buffer_width - x;
			if ( y + h > buffer_height ) h = buffer_height - y;

			if ( fill ) {
				for ( uint32_t iy = y; iy < y + h; ++iy ) {
					for ( uint32_t ix = x; ix < x + w; ++ix ) {
						buffer::set_pixel( color, ix, iy, z );
					}
				}
			} else {
				for ( uint32_t ix = 0; ix < w; ++ix ) {
					buffer::set_pixel( color, x + ix, y, z );
					buffer::set_pixel( color, x + ix, y + h - 1, z );
				}
				for ( uint32_t iy = 1; iy < h - 1; ++iy ) {
					buffer::set_pixel( color, x, y + iy, z );
					buffer::set_pixel( color, x + w - 1, y + iy, z );
				}
			}
		}

		void line( uint32_t color, uint32_t x, uint32_t y, uint32_t to_x, uint32_t to_y, uint32_t z ) {
			uint32_t from_x = x;
			uint32_t from_y = y;

			uint32_t dx = abs( static_cast<int32_t>( to_x ) - static_cast<int32_t>( from_x ) );
			int64_t dy = -abs( static_cast<int32_t>( to_y ) - static_cast<int32_t>( from_y ) );
			int8_t sx = from_x < to_x ? +1 : -1;
			int8_t sy = from_y < to_y ? +1 : -1;

			int64_t err = dx + dy;
			int64_t err2;

			while ( true ) {
				buffer::set_pixel( color, from_x, from_y, z );

				if ( from_x == to_x && from_y == to_y ) break;

				err2 = err + err;

				if ( err2 >= dy ) {
					err += dy;
					from_x += sx;
				}
				if ( err2 <= dx ) {
					err += dx;
					from_y += sy;
				}
			}
		}

	}
}