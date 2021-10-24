#pragma once

#include "../head.cpp"


namespace tamachi {
	namespace buffer {

		uint32_t _width = 0;
		uint32_t _height = 0;
		uint64_t _size = 0;
		uint32_t _layers_count = 0;

		void* _bg_memory = nullptr;
		void* _layers_memory = nullptr;

		BITMAPINFO _bmi = {};
		BLENDFUNCTION _blend_function = {};
		
		HDC _bg_hdc;
		HBITMAP _bg_bitmap;
		HDC _layers_hdc;
		HBITMAP _layers_bitmap;

		HWND _window;
		HDC _hdc;

		uint8_t _bg_color = 0x00;

		bool _is_empty = true;
		bool _is_inited = false;
		bool _is_created = false;

		void init();
		void create();
		void destroy();
		void resize( uint32_t new_width, uint32_t new_height, uint32_t new_layers_count );
		void clear( uint8_t byte );
		void set_pixel( uint32_t color, uint32_t x, uint32_t y, uint32_t z );
		void set_bg_color( uint8_t new_bg_color );
		uint32_t get_width();
		uint32_t get_height();
		void render( uint32_t width, uint32_t height );
		void reset();

		void init() {
			if ( _is_inited ) return;

			_is_inited = true;

			_bmi.bmiHeader.biSize = sizeof(_bmi.bmiHeader);
			_bmi.bmiHeader.biWidth = 0;
			_bmi.bmiHeader.biHeight = 0;
			_bmi.bmiHeader.biPlanes = 1;
			_bmi.bmiHeader.biBitCount = 4 * 8;
			_bmi.bmiHeader.biCompression = BI_RGB;

			_blend_function.BlendOp = AC_SRC_OVER;
			_blend_function.BlendFlags = 0;
			_blend_function.SourceConstantAlpha = 0xFF;
			_blend_function.AlphaFormat = AC_SRC_ALPHA;
		}

		void create( HWND window, HDC hdc ) {
			if ( !_is_inited ) return;
			if ( _is_created ) return;

			_is_created = true;

			_window = window;
			_hdc = hdc;

			_bg_hdc = CreateCompatibleDC( _hdc );
			_layers_hdc = CreateCompatibleDC( _hdc );
		}

		void destroy() {
			if ( !_is_inited ) return;
			if ( !_is_created ) return;

			_is_created = false;

			if ( _bg_bitmap ) DeleteObject( _bg_bitmap );
			if ( _bg_memory ) VirtualFree( _bg_memory, 0, MEM_RELEASE );
			if ( _layers_bitmap ) DeleteObject( _layers_bitmap );
			if ( _layers_memory ) VirtualFree( _layers_memory, 0, MEM_RELEASE );

			_width = 0;
			_height = 0;
			_size = 0;
			_layers_count = 0;

			ReleaseDC( _window, _bg_hdc );
			ReleaseDC( _window, _layers_hdc );
		}

		void resize( uint32_t new_width=0, uint32_t new_height=0, uint32_t new_layers_count=0 ) {
			if ( !new_width ) new_width = _width;
			if ( !new_height ) new_height = _height;
			if ( !new_layers_count ) new_layers_count = _layers_count ? _layers_count : 1;

			if ( _bg_bitmap ) DeleteObject( _bg_bitmap );
			if ( _bg_memory ) VirtualFree( _bg_memory, 0, MEM_RELEASE );
			if ( _layers_bitmap ) DeleteObject( _layers_bitmap );
			if ( _layers_memory ) VirtualFree( _layers_memory, 0, MEM_RELEASE );

			_width = new_width;
			_height = new_height;
			_size = _width * _height;

			_bg_memory = VirtualAlloc( 0, _size * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
			set_bg_color( _bg_color );

			_layers_count = new_layers_count;
			_layers_memory = VirtualAlloc( 0, _layers_count * _size * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
			
			_bg_bitmap = CreateCompatibleBitmap( _hdc, _width, _height );
			SelectObject( _bg_hdc, _bg_bitmap );

			_layers_bitmap = CreateCompatibleBitmap( _hdc, _width, _height );
			SelectObject( _layers_hdc, _layers_bitmap );

			_is_changed = true;
			_is_updated = true;
		}

		void clear() {
			if ( !_layers_memory ) return;
			if ( _is_empty ) return;

			memset( _layers_memory, 0, _layers_count * _size * 4 );

			_is_empty = true;
			_is_changed = true;
		}

		void set_pixel( uint32_t color, uint32_t x, uint32_t y, uint32_t z=0 ) {
			if ( !_layers_memory ) return;
			if ( x > _width || y > _height || z >= _layers_count ) return;

			uint64_t shift = y * _width + x;
			uint32_t* pos = reinterpret_cast<uint32_t*>( _layers_memory ) + z * _size;

			uint8_t A = ( color >> 24 ) & 0xFF;
			uint8_t R = ( color >> 16 ) & 0xFF;
			uint8_t G = ( color >>  8 ) & 0xFF;
			uint8_t B = ( color >>  0 ) & 0xFF;

			R = R * A / 0xFF;
			G = G * A / 0xFF;
			B = B * A / 0xFF;

			*( pos + shift ) = A << 24 | R << 16 | G << 8 | B;

			_is_empty = false;
			_is_changed = true;
		}

		void set_bg_color( uint8_t new_bg_color=0x00 ) {
			_bg_color = new_bg_color;

			if ( _bg_memory ) memset( _bg_memory, _bg_color, _size * 4 );

			_is_changed = true;
		}

		uint32_t get_width() { return _width; }
		uint32_t get_height() { return _height; }

		void render( HDC hdc, uint32_t width, uint32_t height ) {
			if ( !_layers_memory ) return;

			if ( !hdc ) hdc = _hdc;

			_bmi.bmiHeader.biWidth = _width;
			_bmi.bmiHeader.biHeight = -_height;

			StretchDIBits( hdc, 0, 0, width, height, 0, 0, _width, _height, _bg_memory, &_bmi, DIB_RGB_COLORS, SRCCOPY );
			
			StretchDIBits( _bg_hdc, 0, 0, _width, _height, 0, 0, _width, _height, _bg_memory, &_bmi, DIB_RGB_COLORS, SRCCOPY );
			
			for ( int64_t z = 0; z < _layers_count; ++z ) {
				void* layer = reinterpret_cast<void*>( reinterpret_cast<uint8_t*>( _layers_memory ) + z * _size * 4 );

				StretchDIBits( _layers_hdc, 0, 0, _width, _height, 0, 0, _width, _height, layer, &_bmi, DIB_RGB_COLORS, SRCCOPY );
				AlphaBlend( _bg_hdc, 0, 0, _width, _height, _layers_hdc, 0, 0, _width, _height, _blend_function );
			}

			double rw = static_cast<double>( width ) / static_cast<double>( _width );
			double rh = static_cast<double>( height ) / static_cast<double>( _height );
			double scale = rw < rh ? rw : rh;

			uint32_t dx = 0.5 * ( width - scale * _width );
			uint32_t dy = 0.5 * ( height - scale * _height );

			StretchBlt( hdc, dx, dy, width - 2 * dx, height - 2 * dy, _bg_hdc, 0, 0, _width, _height, SRCCOPY );
		}

		void reset() {
			if ( !_is_inited ) return;

			destroy();

			_bg_color = 0x00;
			_is_empty = true;

			_is_inited = false;
		}

	}
}