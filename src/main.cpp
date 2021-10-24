#include "game/game.cpp"


int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) {
	tamachi::init( hInstance, lpCmdLine );
	game::init();
	tamachi::start();

	return 0;
}