#pragma once

#include "head.cpp"
#include "hero.cpp"


namespace game {

	tamachi::stage::Stage* _stage = nullptr;

	void init();
	void _on_start( tamachi::stage::Stage* stage );
	void _on_stop( tamachi::stage::Stage* stage );
	void _on_update( double delta );
	void _on_frame( double delta );

	void init() {
		tamachi::on( "start", _on_start );
		tamachi::on( "stop", _on_stop );

		tamachi::image::factory::load( paths_images );

		tamachi::scene::factory::create( "default" );
	}

	void _on_start( tamachi::stage::Stage* stage ) {
		_stage = stage;
		
		_stage->camera->set_scene( tamachi::scene::factory::get( "default" ) );

		tamachi::fps::set_interval( 1.0 );
		tamachi::fps::on_interval([]( auto fps ){
			_stage->set_title( "FPS: " + std::to_string( fps ) );
		});

		_stage->on( "update", _on_update );
		_stage->on( "frame", _on_frame );

		_stage->canvas->set_size( WIDTH, HEIGHT );
		_stage->canvas->set_depth( 5 );

		hero::create( _stage );

		_stage->open( WIDTH, HEIGHT );
	}

	void _on_stop( tamachi::stage::Stage* stage ) {
		hero::destroy();
	}

	void _on_update( double delta ) {
		// todo: on either stage or canvas update
	}

	void _on_frame( double delta ) {
		hero::frame( delta );
	}

}