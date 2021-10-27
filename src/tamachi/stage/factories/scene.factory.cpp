#pragma once

#include "scene.cpp"


namespace tamachi {
	namespace scene {
		namespace factory {

			auto _storage = new Storage<std::string, Scene>();

			Scene* create( std::string name ) {
				if ( _storage->has( name ) ) return _storage->get( name );

				auto scene = new Scene( name );

				_storage->set( name, scene );

				return scene;
			}

			Scene* get( std::string name ) {
				return _storage->get( name );
			}

			void reset() {
				_storage->clear();
			}

		}
	}
}