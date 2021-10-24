#pragma once


namespace game {

	std::string title = "Game";

	uint32_t WIDTH = 256;
	uint32_t HEIGHT = 144;

	std::vector<std::string> paths_images = {
		"assets/images/pico",
	};

	std::unordered_map<std::string, tamachi::Tile*> tiles;

}