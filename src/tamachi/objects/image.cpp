#pragma once

#include "../head.cpp"


namespace tamachi {

	class Image {
	public:

		Image( std::string filename ) {
			std::ifstream file( ( filename + ".tamachi-image" ).c_str(), std::ios::in | std::ios::binary );

			if ( !file.is_open() ) {
				std::cout << "Could not open file: " << filename << std::endl;
				return;
			}

			uint8_t version = 0;

			file.read( reinterpret_cast<char*>( &version ), sizeof(uint8_t) );

			switch ( version ) {
				case 0: {
					uint8_t char_width, char_height, char_palette_size;

					file.read( reinterpret_cast<char*>( &char_width ), sizeof(uint8_t) );
					file.read( reinterpret_cast<char*>( &char_height ), sizeof(uint8_t) );
					file.read( reinterpret_cast<char*>( &char_palette_size ), sizeof(uint8_t) );

					width = char_width ? char_width : 256;
					height = char_height ? char_height : 256;
					uint32_t palette_size = char_palette_size ? char_palette_size : 256;

					uint8_t R, G, B, A;
					uint32_t color;
					uint32_t palette[256] = {};

					for ( uint8_t i = 0; i < palette_size; ++i ) {
						file.read( reinterpret_cast<char*>( &R ), sizeof(uint8_t) );
						file.read( reinterpret_cast<char*>( &G ), sizeof(uint8_t) );
						file.read( reinterpret_cast<char*>( &B ), sizeof(uint8_t) );
						file.read( reinterpret_cast<char*>( &A ), sizeof(uint8_t) );

						color = 0;
						color = color | A;
						color = ( color << 8 ) | R;
						color = ( color << 8 ) | G;
						color = ( color << 8 ) | B;

						palette[ i ] = color;
					}

					uint8_t pixel;
					uint64_t size = width * height;

					bitmap = VirtualAlloc( 0, width * height * 4, MEM_COMMIT, PAGE_READWRITE );

					auto pos = reinterpret_cast<uint32_t*>( bitmap );

					for ( uint64_t i = 0; i < size; ++i ) {
						file.read( reinterpret_cast<char*>( &pixel ), sizeof(uint8_t) );
						*pos++ = palette[ pixel ];
					}
				} break;
				default: {
					std::cout << "Image version is not supported: " << filename << std::endl;
					return;
				} break;
			}
		}

		~Image() {
			if ( bitmap ) VirtualFree( bitmap, 0, MEM_RELEASE );
		}

		void render( uint32_t x=0, uint32_t y=0, uint32_t z=0 ) {
			if ( !width || !height || !bitmap ) return;

			auto p = reinterpret_cast<uint32_t*>( bitmap );

			uint32_t w = width;
			uint32_t h = height;

			if ( x + w > buffer::width ) w = buffer::width - x;
			if ( y + h > buffer::height ) h = buffer::height - y;

			for ( uint32_t iy = 0; iy < h; ++iy ) {
				for ( uint32_t ix = 0; ix < w; ++ix ) {
					buffer::set_pixel( *p, x + ix, y + iy, z );
					
					p += 1;
				}
			}
		}

	private:
		uint32_t width = 0;
		uint32_t height = 0;
		void* bitmap = NULL;
	
	};

}