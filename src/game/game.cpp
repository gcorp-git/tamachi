#pragma once

#include "head.cpp"
#include "hero.cpp"
#include "map.cpp"


namespace game {

	tamachi::stage::Stage* _stage = nullptr;

	Map* _map = nullptr;
	Hero* _hero = nullptr;

	void init();
	void _on_start( tamachi::stage::Stage* stage );
	void _on_stop( tamachi::stage::Stage* stage );
	void _on_update( double delta );
	void _on_frame( double delta );

	void init() {
		tamachi::on( "start", _on_start );
		tamachi::on( "stop", _on_stop );

		tamachi::scene::factory::create( "default" );
	}

	void _on_start( tamachi::stage::Stage* stage ) {
		_stage = stage;
		
		tamachi::fps::set_interval( 1.0 );
		tamachi::fps::on_interval([]( auto fps ){
			_stage->set_title( "FPS: " + std::to_string( fps ) );
		});

		_stage->on( "update", _on_update );
		_stage->on( "frame", _on_frame );

		_stage->canvas->set_size( WIDTH, HEIGHT );
		_stage->canvas->set_depth( 5 );

		auto scene = tamachi::scene::factory::get( "default" );

		_stage->camera->set_scene( scene );

		_map = new Map( _stage );
		_hero = new Hero( _stage );

		_hero->x = 0;
		_hero->y = 0;
		_hero->z = 3;

		scene->set( _map );
		scene->set( _hero );

		_stage->open( WIDTH, HEIGHT );
	}

	void _on_stop( tamachi::stage::Stage* stage ) {
		auto scene = tamachi::scene::factory::get( "default" );

		delete scene;

		delete _map;
		delete _hero;
	}

	void _on_update( double delta ) {
		// todo: on either stage or canvas update
	}

	void _on_frame( double delta ) {
		_map->frame( delta );
		_hero->frame( delta );
	}

}