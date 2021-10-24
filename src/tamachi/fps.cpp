#pragma once

#include "head.cpp"


namespace tamachi {
	namespace fps {

		double _duration = 0;
		double _interval = 0;
		
		std::function<void( double )> _handler = nullptr;

		uint64_t _count = 0;
		uint64_t _value = 0;

		void frame( double delta ) {
			_count++;
			_duration += delta;

			if ( _duration > _interval ) {
				_value = _count;
				_count = 0;
				_duration = 0;

				if ( _handler ) _handler( _value );
			}
		}

		uint64_t get() {
			return _value;
		}

		void set_interval( double interval=0 ) {
			_interval = interval;
		}

		void on_interval( std::function<void( double )> handler ) {
			_handler = handler;
		}

	}
}