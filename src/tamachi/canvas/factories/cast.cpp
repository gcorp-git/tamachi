#pragma once


namespace tamachi {

	struct CastState {
		int64_t x;
		int64_t y;
		int64_t z;

		int64_t width;
		int64_t height;

		uint32_t dx;
		uint32_t dy;
		uint32_t dw;
		uint32_t dh;

		bool is_visible;
	};

	struct Cast {
		Tile* tile;

		CastState previous;
		CastState current;
	};

}