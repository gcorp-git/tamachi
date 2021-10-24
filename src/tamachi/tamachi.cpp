#pragma once

#include "cursor.cpp"
#include "fps.cpp"
#include "images.cpp"
#include "paint.cpp"
#include "tiles.cpp"

#include "stage/stage.cpp"


namespace tamachi {

	bool _is_inited = false;
	bool _is_started = false;

	std::function<void()> _on_start = nullptr;
	std::function<void()> _on_stop = nullptr;
	std::function<void()> _on_update = nullptr;
	std::function<void( double )> _on_frame = nullptr;

	void init( HINSTANCE hInstance, LPSTR lpCmdLine );
	void start();
	void stop();
	void frame( double delta );
	void on_start( std::function<void()> handler );
	void on_stop( std::function<void()> handler );
	void on_update( std::function<void()> handler );
	void on_frame( std::function<void( double )> handler );

	void init( HINSTANCE hInstance, LPSTR lpCmdLine ) {
		if ( _is_inited ) return;

		_is_inited = true;

		_hInstance = hInstance;

		stage::init( stop );
	}

	void start() {
		if ( !_is_inited ) return;
		if ( _is_started ) return;

		_is_started = true;

		stage::create( 320, 240 );

		auto id = input::on( "keydown", []( uint32_t vk_code ){
			if ( input::keys[ VK_MENU ] && input::keys[ VK_RETURN ] ) {
				stage::fullscreen::toggle();
			}
		});

		if ( _on_start ) _on_start();

		auto last = std::chrono::steady_clock::now();

		while ( _is_started ) {
			stage::tick();

			auto now = std::chrono::steady_clock::now();
			auto mcs = std::chrono::duration_cast<std::chrono::microseconds>( now - last );
			double delta = static_cast<double>( mcs.count() ) / 1000000;

			if ( delta > 0.015 ) {
				frame( delta );
				last = now;
			}
		}
	}

	void stop() {
		if ( !_is_inited ) return;
		if ( !_is_started ) return;

		_is_started = false;

		if ( _on_stop ) _on_stop();

		cursor::reset();
		stage::reset();
		tiles::reset();
	}

	void frame( double delta ) {
		if ( _is_updated && _on_update ) _on_update();
		if ( _on_frame ) _on_frame( delta );
	
		stage::frame( delta );
		fps::frame( delta );

		_is_changed = false;
		_is_updated = false;
	}

	void on_start( std::function<void()> handler ) { _on_start = handler; }
	void on_stop( std::function<void()> handler ) { _on_stop = handler; }
	void on_update( std::function<void()> handler ) { _on_update = handler; }
	void on_frame( std::function<void( double )> handler ) { _on_frame = handler; }

}