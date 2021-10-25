#pragma once

#include "../head.cpp"


namespace tamachi {

	struct Tile {
		uint64_t id;

		Image* image;

		uint32_t dx;
		uint32_t dy;
		uint32_t dw;
		uint32_t dh;

		uint32_t width;
		uint32_t height;

		uint32_t x;
		uint32_t y;
		uint32_t z;

		bool visible;
	};

}