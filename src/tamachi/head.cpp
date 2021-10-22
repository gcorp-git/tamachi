#pragma once

#include <windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <chrono>
#include <vector>


namespace tamachi {

	LPCTSTR window_title = "Game Title";
	HWND window;
	HDC hdc;
	MSG message;
	BITMAPINFO bmi;

	bool is_running = false;
	bool is_changed = false;

	std::function<void( double )> _on_frame;

}