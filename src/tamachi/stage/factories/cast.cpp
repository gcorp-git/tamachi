#pragma once


namespace tamachi {

	struct CastState {
		uint32_t x;
		uint32_t y;
		uint32_t z;

		uint32_t width;
		uint32_t height;

		uint32_t dx;
		uint32_t dy;
		uint32_t dw;
		uint32_t dh;

		bool visible;
	};

	struct Cast {
		Tile* tile;

		CastState previous;
		CastState current;
	};

}