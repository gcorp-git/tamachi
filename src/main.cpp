#include "game/game.cpp"


int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) {
	game::init( hInstance, lpCmdLine );

	game::start();
	game::stop();

	return 0;
}