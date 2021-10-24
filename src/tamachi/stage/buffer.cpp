#pragma once

#include "../head.cpp"


namespace tamachi {
	namespace buffer {

		uint32_t _width = 0;
		uint32_t _height = 0;
		uint64_t _size = 0;
		uint32_t _layers_count = 0;

		void* _bg_memory = nullptr;

		BITMAPINFO _bmi = {};
		BLENDFUNCTION _blend_function = {};
		
		HDC _bg_hdc;
		HBITMAP _bg_bitmap;

		std::vector<HDC> _layers_hdcs = {};

		HWND _window;
		HDC _hdc;

		uint8_t _bg_color = 0x00;

		uint32_t _last_render_width = 0;
		uint32_t _last_render_height = 0;

		bool _is_empty = true;
		bool _is_inited = false;
		bool _is_created = false;

		void init();
		void create();
		void destroy();
		void resize( uint32_t new_width, uint32_t new_height, uint32_t new_layers_count );
		void clear( uint32_t width, uint32_t height, uint32_t x, uint32_t y, uint32_t z );
		void draw_bitmap(  void* memory, uint32_t width, uint32_t height, uint32_t x, uint32_t y, uint32_t z );
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
		}

		void destroy() {
			if ( !_is_inited ) return;
			if ( !_is_created ) return;

			_is_created = false;

			DeleteDC( _bg_hdc );

			if ( _bg_bitmap ) DeleteObject( _bg_bitmap );
			if ( _bg_memory ) VirtualFree( _bg_memory, 0, MEM_RELEASE );
			
			HDC layer_hdc;
			HGDIOBJ prev_bitmap;

			for ( uint32_t z = 0; z < _layers_count; ++z ) {
				layer_hdc = _layers_hdcs[ z ];
				prev_bitmap = GetCurrentObject( layer_hdc, OBJ_BITMAP );

				DeleteDC( layer_hdc );

				if ( prev_bitmap ) DeleteObject( prev_bitmap );
			}

			_layers_hdcs.clear();

			_width = 0;
			_height = 0;
			_size = 0;
			_layers_count = 0;

			_last_render_width = 0;
			_last_render_height = 0;

			_is_changed = true;
			_is_updated = true;
		}

		void resize( uint32_t new_width=0, uint32_t new_height=0, uint32_t new_layers_count=0 ) {
			if ( !new_width ) new_width = _width;
			if ( !new_height ) new_height = _height;
			if ( !new_layers_count ) new_layers_count = _layers_count ? _layers_count : 1;

			if ( _bg_memory ) VirtualFree( _bg_memory, 0, MEM_RELEASE );

			_width = new_width;
			_height = new_height;
			_size = _width * _height;

			_bg_memory = VirtualAlloc( 0, _size * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
			set_bg_color( _bg_color );

			_layers_count = new_layers_count;
			
			HGDIOBJ bitmap;
			HGDIOBJ prev_bitmap;

			_bg_bitmap = CreateCompatibleBitmap( _hdc, _width, _height );
			prev_bitmap = SelectObject( _bg_hdc, _bg_bitmap );
			
			if ( prev_bitmap ) DeleteObject( prev_bitmap );

			for ( auto hdc : _layers_hdcs ) DeleteDC( hdc );

			_layers_hdcs.clear();

			HDC layer_hdc;

			for ( uint32_t z = 0; z < _layers_count; ++z ) {
				if ( z >= _layers_hdcs.size() ) {
					layer_hdc = CreateCompatibleDC( _hdc );
					_layers_hdcs.push_back( layer_hdc );
				}

				bitmap = CreateCompatibleBitmap( _hdc, _width, _height );
				prev_bitmap = SelectObject( _layers_hdcs[ z ], bitmap );

				if ( prev_bitmap ) DeleteObject( prev_bitmap );
			}

			_is_changed = true;
			_is_updated = true;
		}

		void clear( uint32_t width, uint32_t height, uint32_t x, uint32_t y, uint32_t z=0 ) {
			if ( _is_empty ) return;

			_bmi.bmiHeader.biWidth = _width;
			_bmi.bmiHeader.biHeight = -_height;

			StretchDIBits( _layers_hdcs[ z ], x, y, width, height, 0, 0, _width, _height, _bg_memory, &_bmi, DIB_RGB_COLORS, SRCCOPY );

			_bmi.bmiHeader.biWidth = _width;
			_bmi.bmiHeader.biHeight = -_height;

			_is_empty = true;
			_is_changed = true;
		}

		void draw_bitmap( void* memory, uint32_t width, uint32_t height, uint32_t x, uint32_t y, uint32_t z=0 ) {
			_bmi.bmiHeader.biWidth = width;
			_bmi.bmiHeader.biHeight = -height;

			StretchDIBits( _layers_hdcs[ z ], x, y, width, height, 0, 0, width, height, memory, &_bmi, DIB_RGB_COLORS, SRCCOPY );
			
			_bmi.bmiHeader.biWidth = _width;
			_bmi.bmiHeader.biHeight = -_height;

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
			if ( !hdc ) hdc = _hdc;

			_bmi.bmiHeader.biWidth = _width;
			_bmi.bmiHeader.biHeight = -_height;

			if ( _last_render_width != width || _last_render_height != height ) {
				StretchDIBits( hdc, 0, 0, width, height, 0, 0, _width, _height, _bg_memory, &_bmi, DIB_RGB_COLORS, SRCCOPY );
			}

			StretchDIBits( _bg_hdc, 0, 0, _width, _height, 0, 0, _width, _height, _bg_memory, &_bmi, DIB_RGB_COLORS, SRCCOPY );
			
			for ( int64_t z = 0; z < _layers_count; ++z ) {
				AlphaBlend( _bg_hdc, 0, 0, _width, _height, _layers_hdcs[ z ], 0, 0, _width, _height, _blend_function );
			}

			double rw = static_cast<double>( width ) / static_cast<double>( _width );
			double rh = static_cast<double>( height ) / static_cast<double>( _height );
			double scale = rw < rh ? rw : rh;

			uint32_t dx = 0.5 * ( width - scale * _width );
			uint32_t dy = 0.5 * ( height - scale * _height );

			StretchBlt( hdc, dx, dy, width - 2 * dx, height - 2 * dy, _bg_hdc, 0, 0, _width, _height, SRCCOPY );
		
			_last_render_width = width;
			_last_render_height = height;
		}

		void reset() {
			if ( !_is_inited ) return;

			destroy();

			_bg_color = 0x00;
			_is_empty = true;
		}

	}
}