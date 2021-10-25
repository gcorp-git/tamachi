#pragma once


namespace tamachi {

	template<typename T, typename U>
	class Storage {
	public:
		
		bool has( T id ) {
			return _map.find( id ) != _map.end();
		}

		void set( T id, U o ) {
			auto found = _map.find( id );

			if ( found != _map.end() ) delete _map[ id ];

			_map.insert_or_assign( id, o );
		}

		U get( T id ) {
			return has( id ) ? _map[ id ] : nullptr;
		}

		void remove( T id ) {
			auto found = _map.find( id );

			if ( found == _map.end() ) return;

			_map.erase( found );
		}

		void each( std::function<void(U)> handler ) {
			for ( auto it : _map ) handler( it.second );
		}
		
		void clear() {
			for ( auto it : _map ) delete it.second;

			_map.clear();
		}

	protected:
		std::unordered_map<T, U> _map = {};

	};

}