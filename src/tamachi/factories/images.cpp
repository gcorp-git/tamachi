#pragma once

#include "../head.cpp"
#include "../utils/storage.cpp"
#include "image.cpp"


namespace tamachi {
	namespace images {

		auto _storage = new Storage<std::string, Image*>();

		void load( std::vector<std::string> paths ) {
			for ( auto path : paths ) _storage->set( path, new Image( path ) );
		}

		Image* get( std::string path ) {
			return _storage->get( path );
		}

		void reset() {
			_storage->clear();
		}

	}
}