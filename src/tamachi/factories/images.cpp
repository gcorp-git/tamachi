#pragma once

#include "../head.cpp"
#include "../objects/image.cpp"


namespace tamachi {
	namespace images {

		std::unordered_map<std::string, Image*> _storage = {};

		void load( std::vector<std::string> paths ) {
			for ( auto path : paths ) {
				auto found = _storage.find( path );

				if ( found != _storage.end() ) delete _storage[ path ];

				auto image = new Image( path );

				_storage.insert_or_assign( path, image );
			}
		}

		Image* get( std::string path ) {
			auto found = _storage.find( path );

			return found != _storage.end() ? _storage[ path ] : nullptr;
		}

	}
}