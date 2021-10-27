#pragma once


namespace tamachi {
	namespace canvas {
	
		class Buffer {
		public:
			
			Buffer() {
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

				_listeners = new Listeners<bool>();
			}

			~Buffer() {
				detach();

				delete _zeros;
				delete _bg;

				_listeners->clear();

				delete _listeners;
			}

			void attach( HDC hdc ) {
				if ( _is_attached ) detach();

				_is_attached = true;

				_hdc = hdc;

				_is_updated = true;
			}

			void detach() {
				if ( !_is_attached ) return;

				_is_attached = false;

				_remove_layer( _zeros );
				_remove_layer( _bg );

				for ( auto layer : _layers ) _remove_layer( layer );

				_layers.clear();

				_hdc = NULL;
			}

			uint64_t on( std::string event, std::function<void(bool)> listener ) {
				return _listeners->on( event, listener );
			}

			void off( std::string event, uint64_t id ) {
				_listeners->off( event, id );
			}

			void set_size( uint32_t width, uint32_t height ) {
				_config.width = width;
				_config.height = height;

				_is_updated = true;
			}

			void set_depth( uint32_t depth ) {
				_config.depth = depth;

				_is_updated = true;
			}

			void set_bg( uint8_t bg=0x00 ) {
				_config.bg = bg;

				_is_updated = true;
			}

			void draw(
				void* memory, uint32_t memory_width, uint32_t memory_height,
				int64_t x=0, int64_t y=0, int64_t z=0, int64_t width=0, int64_t height=0,
				uint32_t dx=0, uint32_t dy=0, uint32_t dw=0, uint32_t dh=0
			) {
				if ( !_is_attached ) return;
				if ( !memory ) return;
				if ( z < 0 ) return;

				_update();

				if ( !width ) width = memory_width;
				if ( !height ) height = memory_height;

				if ( !dw ) dw = width;
				if ( !dh ) dh = height;

				if ( width <= 0 || height <= 0 ) return;
				if ( x + width <= 0 || y + height <= 0 ) return;
				if ( x >= _config.width || y >= _config.height || z > _config.depth ) return;

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
			}

			void clear( int64_t x, int64_t y, int64_t z, int64_t width, int64_t height ) {
				draw( _zeros->memory, _config.width, _config.height, x, y, z, width, height );
			}

			void flush(
				HDC hdc=NULL, FLUSH_MODE mode=MODE_DEFAULT,
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
					case MODE_STRETCH: {
						StretchBlt( hdc, x, y, width, height, _bg->hdc, dx, dy, dw, dh, SRCCOPY );
					} break;
					case MODE_CENTER: {
						auto pad = get_paddings( { width, height }, { _config.width, _config.height } );

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

		private:
			bool _is_attached = false;
			bool _is_updated = false;

			BufferConfig _config = {};

			BITMAPINFO _bmi = {};
			BLENDFUNCTION _bf = {};
			
			HDC _hdc = NULL;

			Layer* _zeros = nullptr;
			Layer* _bg = nullptr;
			std::vector<Layer*> _layers = {};

			Listeners<bool>* _listeners = nullptr;

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

			void _update() {
				if ( !_is_attached ) return;
				if ( !_is_updated ) return;

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

				_is_updated = false;
			}

		};
	
	}
}