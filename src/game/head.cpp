#pragma once

#include "../tamachi/tamachi.cpp"


namespace game {

	uint32_t WIDTH = 256;
	uint32_t HEIGHT = 144;
	// uint32_t WIDTH = 800;
	// uint32_t HEIGHT = 600;

	std::vector<std::string> paths_images = {
		"assets/images/pico",
		"assets/images/red",
		"assets/images/green",
		"assets/images/blue",
	};

	std::unordered_map<std::string, tamachi::tile::Tile*> tiles;

}