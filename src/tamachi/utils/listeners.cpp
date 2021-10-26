#pragma once

#include "../head.cpp"


namespace tamachi {

	template<typename T>
	class Listeners {
	public:

		Listeners() {
			//
		}
		
		~Listeners() {
			clear();
		}

		uint64_t on( std::string event, std::function<void(T)> listener ) {
			auto fm = _listeners.find( event );

			if ( fm == _listeners.end() ) {
				std::unordered_map<uint64_t, std::function<void(T)>> map;
				_listeners.insert_or_assign( event, map );
			}

			auto id = ++_IDS;

			_listeners[ event ].insert_or_assign( id, listener );

			return id;
		}

		void off( std::string event, uint64_t id ) {
			auto fm = _listeners.find( event );

			if ( fm == _listeners.end() ) return;

			auto map = _listeners[ event ];

			auto fl = map.find( id );

			if ( fl == map.end() ) return;

			map.erase( fl );
		}

		void dispatch( std::string event, T payload ) {
			auto fm = _listeners.find( event );

			if ( fm == _listeners.end() ) return;

			auto map = _listeners[ event ];

			for ( auto it : map ) it.second( payload );
		}

		void clear() {
			for ( auto it : _listeners ) it.second.clear();

			_listeners.clear();
		}

	private:
		uint64_t _IDS = 0;
		std::unordered_map<std::string, std::unordered_map<uint64_t, std::function<void(T)>>> _listeners = {};
	
	};

}