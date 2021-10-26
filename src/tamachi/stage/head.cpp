#pragma once

#include "../head.cpp"


namespace tamachi {
	namespace stage {

		bool _is_changed = false;
		bool _is_updated = false;

		enum FLUSH_MODE {
			MODE_DEFAULT,
			MODE_STRETCH,
			MODE_CENTER,
		};

		struct RectSize {
			uint32_t width;
			uint32_t height;
		};

		struct Paddings {
			uint32_t top;
			uint32_t left;
		};

		double get_scale( RectSize window, RectSize buffer ) {
			double rw = static_cast<double>( window.width ) / static_cast<double>( buffer.width );
			double rh = static_cast<double>( window.height ) / static_cast<double>( buffer.height );
			
			return rw < rh ? rw : rh;
		}

		Paddings get_paddings( RectSize window, RectSize buffer, double scale=0 ) {
			if ( !scale ) scale = get_scale( window, buffer );

			uint32_t top = static_cast<uint32_t>( 0.5 * ( window.height - scale * buffer.height ) );
			uint32_t left = static_cast<uint32_t>( 0.5 * ( window.width - scale * buffer.width ) );

			return { top, left };
		}

	}
}