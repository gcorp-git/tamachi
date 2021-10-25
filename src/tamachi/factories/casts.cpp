#pragma once

#include "../head.cpp"
#include "../utils/listeners.cpp"
#include "tile.cpp"
#include "cast.cpp"


namespace tamachi {
	namespace casts {

		struct Config {
			uint32_t depth;
			bool is_updated;
		};

		Config _config = {};

		auto _storage = new Storage<uint64_t, Cast*>();
		std::vector<std::unordered_map<uint64_t, Cast*>*> _layers = {};

		auto _listeners = new Listeners<bool>();

		void set( Tile* tile );
		void unset( Tile* tile );
		void refresh();
		void each( std::function<void(std::unordered_map<uint64_t, Cast*>*)> handler );
		void set_depth( uint32_t depth );
		uint64_t on( std::string event, std::function<void(bool)> listener );
		void off( std::string event, uint64_t id );
		void clear();
		void reset();
		void _create( Tile* tile );
		void _remove( Cast* cast );
		void _update( Cast* cast, Tile* tile );
		void _refresh( Cast* cast );

		void set( Tile* tile ) {
			if ( !tile ) return;

			auto cast = _storage->get( tile->id );

			if ( !cast ) { _create( tile ); return; }

			_remove( cast );
			_update( cast, tile );
		}

		void unset( Tile* tile ) {
			if ( !tile ) return;

			auto cast = _storage->get( tile->id );

			if ( !cast ) return;

			_remove( cast );
			_storage->remove( cast->tile->id );
		}

		void refresh() {
			_storage->each( _refresh );
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

		uint64_t on( std::string event, std::function<void(bool)> listener ) {
			return _listeners->on( event, listener );
		}

		void off( std::string event, uint64_t id ) {
			_listeners->off( event, id );
		}

		void clear() {
			for ( auto layer : _layers ) layer->clear();
		}

		void reset() {
			clear();

			_layers.clear();
			_storage->clear();

			_config = {};
		}

		void _create( Tile* tile ) {
			if ( !tile ) return;

			auto cast = new Cast();

			cast->tile = tile;

			_storage->set( cast->tile->id, cast );

			_update( cast, tile );
		}

		void _remove( Cast* cast ) {
			auto layer = _layers[ cast->current.z ];

			auto found = layer->find( cast->tile->id );

			if ( found == layer->end() ) return;

			layer->erase( found );
		}

		void _update( Cast* cast, Tile* tile ) {
			if ( tile->z >= _layers.size() ) return;
			
			if ( !tile->visible && !cast->previous.visible ) return;

			cast->current.x = tile->x;
			cast->current.y = tile->y;
			cast->current.z = tile->z;
			cast->current.dx = tile->dx;
			cast->current.dy = tile->dy;
			cast->current.dw = tile->dw;
			cast->current.dh = tile->dh;
			cast->current.width = tile->width;
			cast->current.height = tile->height;
			cast->current.visible = tile->visible;

			bool same_x = cast->current.x == cast->previous.x;
			bool same_y = cast->current.y == cast->previous.y;
			bool same_z = cast->current.z == cast->previous.z;
			bool same_dx = cast->current.dx == cast->previous.dx;
			bool same_dy = cast->current.dy == cast->previous.dy;
			bool same_dw = cast->current.dw == cast->previous.dw;
			bool same_dh = cast->current.dh == cast->previous.dh;
			bool same_width = cast->current.width == cast->previous.width;
			bool same_height = cast->current.height == cast->previous.height;
			bool same_visible = cast->current.visible == cast->previous.visible;
			
			bool nothing_changed = true;

			nothing_changed = nothing_changed && same_x && same_y && same_z;
			nothing_changed = nothing_changed && same_width && same_height;
			nothing_changed = nothing_changed && same_dx && same_dy && same_dw && same_dh;
			nothing_changed = nothing_changed && same_visible;

			if ( nothing_changed ) return;

			_layers[ cast->current.z ]->insert_or_assign( cast->tile->id, cast );
		}

		void _refresh( Cast* cast ) {
			if ( cast->tile->z >= _layers.size() ) return;
			if ( !cast->previous.visible ) return;

			_layers[ cast->current.z ]->insert_or_assign( cast->tile->id, cast );
		}

	}
}