#pragma once

#include "space/comb.cpp"
#include "space/grid.cpp"
#include "space/map.cpp"
#include "stage/stage.cpp"
#include "utils/fps.cpp"


namespace tamachi {

	bool _is_inited = false;
	bool _is_started = false;

	std::function<void()> _on_start = NULL;
	std::function<void()> _on_stop = NULL;
	std::function<void( double )> _on_frame = NULL;

	void init( HINSTANCE hInstance, LPSTR lpCmdLine );
	void start();
	void stop();
	void frame( double delta );
	void on_start( std::function<void()> handler );
	void on_stop( std::function<void()> handler );
	void on_frame( std::function<void( double )> handler );

	void init( HINSTANCE hInstance, LPSTR lpCmdLine ) {
		if ( _is_inited ) return;

		_is_inited = true;

		_hInstance = hInstance;

		stage::init();
		stage::on( "close", []( auto nothing ){
			stop();
		});
	}

	void start() {
		if ( !_is_inited ) return;
		if ( _is_started ) return;

		_is_started = true;

		if ( _on_start ) _on_start();

		auto last = std::chrono::steady_clock::now();

		while ( _is_started ) {
			stage::tick();

			auto now = std::chrono::steady_clock::now();
			auto mcs = std::chrono::duration_cast<std::chrono::microseconds>( now - last );
			double delta = static_cast<double>( mcs.count() ) / 1000000;

			if ( delta > 0.015 ) {
				if ( _on_frame ) _on_frame( delta );
			
				stage::frame( delta );
				fps::frame( delta );
				
				last = now;
			}
		}
	}

	void stop() {
		if ( !_is_inited ) return;
		if ( !_is_started ) return;

		_is_started = false;

		if ( _on_stop ) _on_stop();

		stage::reset();
	}

	void on_start( std::function<void()> handler ) { _on_start = handler; }
	void on_stop( std::function<void()> handler ) { _on_stop = handler; }
	void on_frame( std::function<void( double )> handler ) { _on_frame = handler; }

}