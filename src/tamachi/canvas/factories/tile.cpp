#pragma once


namespace tamachi {

	struct Tile {
		uint64_t id;

		image::Image* image;

		uint32_t dx;
		uint32_t dy;
		uint32_t dw;
		uint32_t dh;

		uint32_t width;
		uint32_t height;
	};

}