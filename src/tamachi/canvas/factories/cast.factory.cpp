#pragma once

#include "tile.cpp"
#include "cast.cpp"


namespace tamachi {
	namespace canvas {
		
		class CastFactory {
		public:

			CastFactory() {
				_listeners = new Listeners<bool>();
				_storage = new Storage<uint64_t, Cast>();
			}

			~CastFactory() {
				clear();

				_layers.clear();

				_storage->clear();

				delete _listeners;
				delete _storage;
			}
			
			uint64_t on( std::string event, std::function<void(bool)> listener ) {
				return _listeners->on( event, listener );
			}

			void off( std::string event, uint64_t id ) {
				_listeners->off( event, id );
			}
		
			void set( Tile* tile, bool is_visible, int64_t x=0, int64_t y=0, int64_t z=0 ) {
				if ( !tile ) return;

				auto cast = _storage->get( tile->id );

				if ( !cast ) {
					cast = _create( tile );
				} else {
					_remove( cast );
				}

				_update( cast, tile, is_visible, x, y, z );
			}

			void unset( Tile* tile ) {
				if ( !tile ) return;

				auto cast = _storage->get( tile->id );

				if ( !cast ) return;

				_remove( cast );

				auto id = cast->tile->id;

				cast->tile = nullptr;
				cast->previous = {};
				cast->current = {};

				_storage->destroy( id );
			}

			void show( Tile* tile, int64_t x=0, int64_t y=0, int64_t z=0 ) {
				set( tile, true, x, y, z );
			}

			void hide( Tile* tile ) {
				set( tile, false );
			}

			void refresh() {
				_storage->each([ this ]( Cast* cast ){
					if ( cast->current.z >= _layers.size() ) return;
					if ( !cast->previous.is_visible ) return;

					_layers[ cast->current.z ]->insert_or_assign( cast->tile->id, cast );
				});
			}

			void clear() {
				for ( auto layer : _layers ) layer->clear();
			}

			void each( std::function<void(std::unordered_map<uint64_t, Cast*>*)> handler ) {
				for ( auto layer : _layers ) handler( layer );
			}

			void set_depth( uint32_t depth ) {
				_config.depth = depth;

				while ( _layers.size() > _config.depth ) {
					_layers[ _layers.size() - 1 ]->clear();
					_layers.pop_back();
				}

				while ( _layers.size() < _config.depth ) {
					_layers.push_back( new std::unordered_map<uint64_t, Cast*>() );
				}

				_listeners->dispatch( "update", false );
			}

		private:
			CastFactoryConfig _config = {};
		
			Listeners<bool>* _listeners = nullptr;
			Storage<uint64_t, Cast>* _storage = nullptr;

			std::vector<std::unordered_map<uint64_t, Cast*>*> _layers = {};

			Cast* _create( Tile* tile ) {
				if ( !tile ) return nullptr;

				auto cast = _storage->create();

				cast->tile = tile;

				_storage->set( cast->tile->id, cast );

				return cast;
			}

			void _remove( Cast* cast ) {
				if ( !cast ) return;

				auto layer = _layers[ cast->current.z ];

				auto found = layer->find( cast->tile->id );

				if ( found == layer->end() ) return;

				layer->erase( found );
			}

			void _update( Cast* cast, Tile* tile, bool is_visible, int64_t x, int64_t y, int64_t z ) {
				if ( !cast || !tile ) return;
				if ( z >= _layers.size() ) return;
				if ( !is_visible && !cast->previous.is_visible ) return;

				cast->current.x = x;
				cast->current.y = y;
				cast->current.z = z;
				cast->current.dx = tile->dx;
				cast->current.dy = tile->dy;
				cast->current.dw = tile->dw;
				cast->current.dh = tile->dh;
				cast->current.width = tile->width;
				cast->current.height = tile->height;
				cast->current.is_visible = is_visible;

				bool same_x = cast->current.x == cast->previous.x;
				bool same_y = cast->current.y == cast->previous.y;
				bool same_z = cast->current.z == cast->previous.z;
				bool same_dx = cast->current.dx == cast->previous.dx;
				bool same_dy = cast->current.dy == cast->previous.dy;
				bool same_dw = cast->current.dw == cast->previous.dw;
				bool same_dh = cast->current.dh == cast->previous.dh;
				bool same_width = cast->current.width == cast->previous.width;
				bool same_height = cast->current.height == cast->previous.height;
				bool same_is_visible = cast->current.is_visible == cast->previous.is_visible;
				
				bool nothing_changed = true;

				nothing_changed = nothing_changed && same_x && same_y && same_z;
				nothing_changed = nothing_changed && same_width && same_height;
				nothing_changed = nothing_changed && same_dx && same_dy && same_dw && same_dh;
				nothing_changed = nothing_changed && same_is_visible;

				if ( nothing_changed ) return;

				_layers[ cast->current.z ]->insert_or_assign( cast->tile->id, cast );
			}

		};
	
	}
}