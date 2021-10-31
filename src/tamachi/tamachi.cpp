#pragma once

#include "head.cpp"
#include "canvas/canvas.cpp"
#include "space/comb.cpp"
#include "space/grid.cpp"
#include "space/map.cpp"
#include "stage/factory.cpp"


namespace tamachi {

	bool _is_inited = false;
	bool _is_started = false;

	stage::Stage* _stage = nullptr;
	Listeners<stage::Stage*>* _listeners = nullptr;

	void init( HINSTANCE hInstance, LPSTR lpCmdLine );
	Listener on( std::string event, std::function<void(stage::Stage*)> handler );
	void off( Listener listener );
	void start();
	void stop();
	void _loop();

	void init( HINSTANCE instance, LPSTR lpCmdLine ) {
		if ( _is_inited ) return;

		_is_inited = true;

		stage::factory::init( instance );

		_stage = stage::factory::create();
		_listeners = new Listeners<stage::Stage*>();
	}

	Listener on( std::string event, std::function<void(stage::Stage*)> handler ) {
		return _listeners->on( event, handler );
	}

	void off( Listener listener ) {
		_listeners->off( listener );
	}

	void start() {
		if ( !_is_inited ) return;
		if ( _is_started ) return;

		_is_started = true;

		_stage->on( "close", []( auto nothing ){
			stop();
		});

		_listeners->dispatch( "start", _stage );

		_loop();
	}

	void stop() {
		if ( !_is_inited ) return;
		if ( !_is_started ) return;

		_is_started = false;

		_listeners->dispatch( "stop", _stage );

		_stage->close();
	}

	void _loop() {
		auto last = std::chrono::steady_clock::now();

		while ( _is_started ) {
			_stage->tick();

			auto now = std::chrono::steady_clock::now();
			auto mcs = ( std::chrono::duration_cast<std::chrono::microseconds>( now - last ) ).count();
			
			double delta = static_cast<double>( mcs ) / 1000000;

			if ( delta > 0.015 ) {
				_stage->frame( delta );

				last = now;
			} else {
				if ( delta < 0.014 ) Sleep( 1 );
			}
		}
	}

}