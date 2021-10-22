#pragma once

#include "head.cpp"
#include "screen.cpp"


namespace tamachi {
	namespace buffer {

		uint64_t size = 0;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t layers_count = 0;
		uint8_t bg_color = 0x00;
		bool is_empty = true;

		void* bg = NULL;
		void* layers = NULL;

		HDC main_hdc;
		HDC layer_hdc;
		HBITMAP main_bitmap;
		HBITMAP layer_bitmap;
		BLENDFUNCTION blend_function;

		void init() {
			main_hdc = CreateCompatibleDC( hdc );
			layer_hdc = CreateCompatibleDC( hdc );

			blend_function = {};
			blend_function.BlendOp = AC_SRC_OVER;
			blend_function.BlendFlags = 0;
			blend_function.SourceConstantAlpha = 0xFF;
			blend_function.AlphaFormat = AC_SRC_ALPHA;
		}

		void set_bg_color( uint8_t new_bg_color=0x00 ) {
			bg_color = new_bg_color;
			memset( bg, bg_color, size * 4 );
			is_changed = true;
		}

		void reset( uint32_t new_width=320, uint32_t new_height=240, uint32_t new_layers_count=0 ) {
			if ( !new_layers_count ) {
				new_layers_count = layers_count;

				if ( !new_layers_count ) new_layers_count = 1;
			}

			if ( bg ) VirtualFree( bg, 0, MEM_RELEASE );
			if ( layers ) VirtualFree( layers, 0, MEM_RELEASE );
			if ( layer_bitmap ) DeleteObject( layer_bitmap );

			width = new_width;
			height = new_height;
			size = width * height;

			bg = VirtualAlloc( 0, size * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
			
			set_bg_color( 0x00 );

			layers = VirtualAlloc( 0, new_layers_count * size * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
			layers_count = new_layers_count;

			main_bitmap = CreateCompatibleBitmap( hdc, width, height );
			SelectObject( main_hdc, main_bitmap );

			layer_bitmap = CreateCompatibleBitmap( hdc, width, height );
			SelectObject( layer_hdc, layer_bitmap );

			is_changed = true;
		}

		void flush() {
			if ( !layers ) return;

			StretchDIBits( main_hdc, 0, 0, width, height, 0, 0, width, height, bg, &bmi, DIB_RGB_COLORS, SRCCOPY );
			
			for ( int64_t z = 0; z < layers_count; ++z ) {
				void* layer = reinterpret_cast<void*>( reinterpret_cast<uint8_t*>( layers ) + z * size * 4 );

				StretchDIBits( layer_hdc, 0, 0, width, height, 0, 0, width, height, layer, &bmi, DIB_RGB_COLORS, SRCCOPY );
				AlphaBlend( main_hdc, 0, 0, width, height, layer_hdc, 0, 0, width, height, blend_function );
			}

			StretchBlt( hdc, 0, 0, screen::width, screen::height, main_hdc, 0, 0, width, height, SRCCOPY );
			
			is_changed = false;
		}

		void clear( uint8_t byte=0 ) {
			if ( !layers ) return;
			if ( is_empty ) return;

			memset( layers, byte, layers_count * size * 4 );

			is_empty = true;
			is_changed = true;
		}

		void set_pixel( uint32_t color, uint32_t x, uint32_t y, uint32_t z=0 ) {
			if ( !layers ) return;
			if ( x > width || y > height ) return;

			uint64_t shift = y * width + x;
			uint32_t* pos = reinterpret_cast<uint32_t*>( layers ) + z * size;

			uint8_t A = ( color >> 24 ) & 0xFF;
			uint8_t R = ( color >> 16 ) & 0xFF;
			uint8_t G = ( color >>  8 ) & 0xFF;
			uint8_t B = ( color >>  0 ) & 0xFF;

			R = R * A / 0xFF;
			G = G * A / 0xFF;
			B = B * A / 0xFF;

			*( pos + shift ) = A << 24 | R << 16 | G << 8 | B;

			is_empty = false;
			is_changed = true;
		}

	}
}