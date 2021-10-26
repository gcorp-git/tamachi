#pragma once

#include "../head.cpp"


namespace tamachi {

	template<typename T, typename U>
	class Storage {
	public:

		Storage( uint64_t free_limit=0 ) {
			_free_limit = free_limit;
		}

		~Storage() {
			for ( auto it : _map ) delete it.second;

			_map.clear();

			for ( auto it : _free ) delete it;

			_free.clear();
		}

		U* create() {
			auto free_size = _free.size();

			if ( free_size ) {
				auto o = _free[ free_size - 1 ];
			
				_free.pop_back();

				return o;
			}

			return new U();
		}

		void destroy( T id ) {
			auto found = _map.find( id );

			if ( found == _map.end() ) return;

			U* o = _map[ id ];

			_map.erase( found );

			if ( _free.size() < _free_limit ) {
				_free.push_back( o );
			} else {
				delete o;
			}
		}
		
		bool has( T id ) {
			return _map.find( id ) != _map.end();
		}

		void set( T id, U* o ) {
			destroy( id );

			_map.insert_or_assign( id, o );
		}

		U* get( T id ) {
			return has( id ) ? _map[ id ] : nullptr;
		}

		void remove( T id ) {
			auto found = _map.find( id );

			if ( found == _map.end() ) return;

			_map.erase( found );
		}

		void each( std::function<void(U*)> handler ) {
			for ( auto it : _map ) handler( it.second );
		}
		
		void clear() {
			for ( auto it : _map ) destroy( it.first );

			_map.clear();
		}

	protected:
		uint64_t _free_limit = 0;
		std::unordered_map<T, U*> _map = {};
		std::vector<U*> _free = {};

	};

}