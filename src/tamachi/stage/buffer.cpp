#pragma once

#include "head.cpp"
#include "../utils/listeners.cpp"


namespace tamachi {
	namespace buffer {

		struct Config {
			uint32_t width;
			uint32_t height;
			uint32_t depth;
			uint8_t bg;
			bool is_updated;
		};

		struct Layer {
			HDC hdc;
			HBITMAP bitmap;
			void* memory;
			bool is_created;
		};

		Config _config = {};

		bool _is_inited = false;
		bool _is_attached = false;
		bool _is_empty = false;

		BITMAPINFO _bmi = {};
		BLENDFUNCTION _bf = {};
		
		HDC _hdc = NULL;

		Layer* _zeros = nullptr;
		Layer* _bg = nullptr;
		std::vector<Layer*> _layers = {};

		auto _listeners = new Listeners<bool>();

		void init();
		void attach( HDC hdc );
		void detach();
		void draw(
			void* memory, uint32_t memory_width, uint32_t memory_height,
			uint32_t x, uint32_t y, uint32_t z, uint32_t width, uint32_t height,
			uint32_t dx, uint32_t dy, uint32_t dw, uint32_t dh
		);
		void clear( uint32_t x, uint32_t y, uint32_t z, uint32_t width, uint32_t height );
		void flush(
			HDC hdc, stage::FLUSH_MODE mode,
			uint32_t x, uint32_t y, uint32_t width, uint32_t height,
			uint32_t dx, uint32_t dy, uint32_t dw, uint32_t dh
		);
		void set_size( uint32_t width, uint32_t height );
		void set_depth( uint32_t depth );
		void set_bg( uint8_t bg );
		uint64_t on( std::string event, std::function<void(bool)> listener );
		void off( std::string event, uint64_t id );
		void reset();
		void _update();
		void _create_layer( Layer* layer );
		void _remove_layer( Layer* layer );
		void _fill_layer( Layer* layer, uint8_t byte );

		void init() {
			if ( _is_inited ) return;

			_is_inited = true;

			_bmi.bmiHeader.biSize = sizeof(_bmi.bmiHeader);
			_bmi.bmiHeader.biWidth = 0;
			_bmi.bmiHeader.biHeight = 0;
			_bmi.bmiHeader.biPlanes = 1;
			_bmi.bmiHeader.biBitCount = 4 * 8;
			_bmi.bmiHeader.biCompression = BI_RGB;

			_bf.BlendOp = AC_SRC_OVER;
			_bf.BlendFlags = 0;
			_bf.SourceConstantAlpha = 0xFF;
			_bf.AlphaFormat = AC_SRC_ALPHA;

			_zeros = new Layer();
			_bg = new Layer();
		}

		void attach( HDC hdc ) {
			if ( !_is_inited ) return;

			detach();

			_is_attached = true;

			_hdc = hdc;
		}

		void detach() {
			if ( !_is_inited ) return;

			_is_attached = false;

			_remove_layer( _zeros );
			_remove_layer( _bg );

			for ( auto layer : _layers ) _remove_layer( layer );

			_layers.clear();

			_hdc = NULL;
		}

		void draw(
			void* memory, uint32_t memory_width, uint32_t memory_height,
			uint32_t x=0, uint32_t y=0, uint32_t z=0, uint32_t width=0, uint32_t height=0,
			uint32_t dx=0, uint32_t dy=0, uint32_t dw=0, uint32_t dh=0
		) {
			if ( !_is_attached ) return;
			if ( !memory ) return;

			if ( !width ) width = memory_width;
			if ( !height ) height = memory_height;

			if ( x >= _config.width || y >= _config.height ) return;
			if ( z > _config.depth ) return;
			if ( !width || !height ) return;

			if ( x + width > _config.width ) width = _config.width - x;
			if ( y + height > _config.height ) height = _config.height - y;

			if ( !dw ) dw = width;
			if ( !dh ) dh = height;

			_update();

			auto pw = _bmi.bmiHeader.biWidth;
			auto ph = _bmi.bmiHeader.biHeight;

			_bmi.bmiHeader.biWidth = memory_width;
			_bmi.bmiHeader.biHeight = -memory_height;

			StretchDIBits(
				_layers[ z ]->hdc,
				x, y, width, height,
				dx, dy, dw, dh,
				memory,
				&_bmi, DIB_RGB_COLORS, SRCCOPY
			);

			_bmi.bmiHeader.biWidth = pw;
			_bmi.bmiHeader.biHeight = ph;

			stage::_is_changed = true;
		}

		void clear( uint32_t x, uint32_t y, uint32_t z, uint32_t width, uint32_t height ) {
			draw( _zeros->memory, _config.width, _config.height, x, y, z, width, height );
		}

		void flush(
			HDC hdc=NULL, stage::FLUSH_MODE mode=stage::MODE_DEFAULT,
			uint32_t x=0, uint32_t y=0, uint32_t width=0, uint32_t height=0,
			uint32_t dx=0, uint32_t dy=0, uint32_t dw=0, uint32_t dh=0
		) {
			if ( !_is_attached ) return;
			
			_update();

			if ( !hdc ) hdc = _hdc;

			if ( !dw ) dw = _config.width;
			if ( !dh ) dh = _config.height;

			auto pw = _bmi.bmiHeader.biWidth;
			auto ph = _bmi.bmiHeader.biHeight;

			_bmi.bmiHeader.biWidth = _config.width;
			_bmi.bmiHeader.biHeight = -_config.height;

			StretchDIBits( _bg->hdc, dx, dy, dw, dh, dx, dy, dw, dh, _bg->memory, &_bmi, DIB_RGB_COLORS, SRCCOPY );
			
			for ( auto layer : _layers ) {
				AlphaBlend( _bg->hdc, dx, dy, dw, dh, layer->hdc, dx, dy, dw, dh, _bf );
			}

			switch ( mode ) {
				case stage::MODE_STRETCH: {
					StretchBlt( hdc, x, y, width, height, _bg->hdc, dx, dy, dw, dh, SRCCOPY );
				} break;
				case stage::MODE_CENTER: {
					auto pad = stage::get_paddings( { width, height }, { _config.width, _config.height } );

					StretchDIBits( hdc, x, y, pad.left, height, 0, 0, 1, 1, _zeros->memory, &_bmi, DIB_RGB_COLORS, SRCCOPY );
					StretchDIBits( hdc, x + width - pad.left, y, pad.left, height, 0, 0, 1, 1, _zeros->memory, &_bmi, DIB_RGB_COLORS, SRCCOPY );
					StretchDIBits( hdc, x, y, width, pad.top, 0, 0, 1, 1, _zeros->memory, &_bmi, DIB_RGB_COLORS, SRCCOPY );
					StretchDIBits( hdc, x, y + height - pad.top, width, pad.top, 0, 0, 1, 1, _zeros->memory, &_bmi, DIB_RGB_COLORS, SRCCOPY );
					
					StretchBlt( hdc, x + pad.left, y + pad.top, width - ( pad.left + pad.left ), height - ( pad.top + pad.top ), _bg->hdc, dx, dy, dw, dh, SRCCOPY );
				} break;
				default: {
					StretchBlt( hdc, x, y, dw, dh, _bg->hdc, dx, dy, dw, dh, SRCCOPY );
				} break;
			}

			_bmi.bmiHeader.biWidth = pw;
			_bmi.bmiHeader.biHeight = ph;
		}

		void set_size( uint32_t width, uint32_t height ) {
			if ( !_is_inited ) return;

			_config.width = width;
			_config.height = height;
			_config.is_updated = true;

			stage::_is_updated = true;
		}

		void set_depth( uint32_t depth ) {
			if ( !_is_inited ) return;

			_config.depth = depth;
			_config.is_updated = true;

			stage::_is_updated = true;
		}

		void set_bg( uint8_t bg=0x00 ) {
			if ( !_is_inited ) return;

			_config.bg = bg;
			_config.is_updated = true;

			stage::_is_updated = true;
		}

		uint64_t on( std::string event, std::function<void(bool)> listener ) {
			return _listeners->on( event, listener );
		}

		void off( std::string event, uint64_t id ) {
			_listeners->off( event, id );
		}

		void reset() {
			if ( !_is_inited ) return;

			detach();

			_is_attached = false;
			_is_empty = false;
			_config = {};
			_hdc = NULL;
			_zeros = nullptr;
			_bg = nullptr;
		}

		void _update() {
			if ( !_is_attached ) return;
			if ( !_config.is_updated ) return;

			_create_layer( _zeros );
			_fill_layer( _zeros, 0x00 );

			_create_layer( _bg );
			_fill_layer( _bg, _config.bg );

			for ( auto layer : _layers ) _remove_layer( layer );

			_layers.clear();

			for ( uint32_t z = 0; z < _config.depth; ++z ) {
				auto layer = new Layer();
				_create_layer( layer );
				_layers.push_back( layer );
			}

			_listeners->dispatch( "update", false );

			_config.is_updated = false;
		}

		void _create_layer( Layer* layer ) {
			if ( layer->is_created ) _remove_layer( layer );

			layer->hdc = CreateCompatibleDC( _hdc );
			layer->bitmap = CreateCompatibleBitmap( _hdc, _config.width, _config.height );
			layer->memory = VirtualAlloc( 0, _config.width * _config.height * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
			
			SelectObject( layer->hdc, layer->bitmap );

			layer->is_created = true;
		}

		void _remove_layer( Layer* layer ) {
			if ( !layer->is_created ) return;

			DeleteDC( layer->hdc );
			DeleteObject( layer->bitmap );
			VirtualFree( layer->memory, 0, MEM_RELEASE );

			layer->is_created = false;
		}

		void _fill_layer( Layer* layer, uint8_t byte=0x00 ) {
			if ( !layer->is_created ) return;

			memset( layer->memory, byte, _config.width * _config.height * 4 );
		}

	}
}