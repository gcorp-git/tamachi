#pragma once


namespace tamachi {
	namespace fps {

		uint32_t _interval = 0;
		std::function<void( double )> _handler;
		std::chrono::steady_clock::time_point _last;
		uint64_t _count = 0;
		uint64_t _value = 0;

		void start( uint32_t interval=0, std::function<void( double )> handler=NULL ) {
			_interval = interval;
			_handler = handler;
			_last = std::chrono::steady_clock::now();
		}

		void frame() {
			_count++;

			auto now = std::chrono::steady_clock::now();
			auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( now - _last );

			if ( delta.count() > _interval ) {
				_value = _count;
				_count = 0;
				_last = now;

				if ( _handler ) _handler( _value );
			}
		}

		uint64_t get() {
			return _value;
		}

	}
}