#pragma once

#include "../utils/storage.cpp"
#include "../utils/listeners.cpp"


namespace tamachi {
	namespace canvas {

		enum FLUSH_MODE {
			MODE_DEFAULT,
			MODE_STRETCH,
			MODE_CENTER,
		};

		struct BufferConfig {
			uint32_t width;
			uint32_t height;
			uint32_t depth;
			uint8_t bg;
		};

		struct Layer {
			HDC hdc;
			HBITMAP bitmap;
			void* memory;
			bool is_created;
		};

		struct RectSize {
			uint32_t width;
			uint32_t height;
		};

		struct Paddings {
			uint32_t top;
			uint32_t left;
		};

		double get_scale( RectSize stage, RectSize canvas ) {
			double rw = static_cast<double>( stage.width ) / static_cast<double>( canvas.width );
			double rh = static_cast<double>( stage.height ) / static_cast<double>( canvas.height );
			
			return rw < rh ? rw : rh;
		}

		Paddings get_paddings( RectSize stage, RectSize canvas, double scale=0 ) {
			if ( !scale ) scale = get_scale( stage, canvas );

			uint32_t top = static_cast<uint32_t>( 0.5 * ( stage.height - scale * canvas.height ) );
			uint32_t left = static_cast<uint32_t>( 0.5 * ( stage.width - scale * canvas.width ) );

			return { top, left };
		}

	}
}