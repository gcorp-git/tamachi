#pragma once

#include <windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <chrono>
#include <vector>
#include <unordered_map>


namespace tamachi {

	LPCTSTR window_title = "Game Title";
	HWND window;
	HDC hdc;
	MSG message;
	BITMAPINFO bmi;

	bool _is_inited = false;
	bool _is_running = false;
	bool _is_changed = false;
	bool _is_updated = false;

	std::function<void()> _on_start;
	std::function<void()> _on_stop;
	std::function<void()> _on_update;
	std::function<void( double )> _on_frame;

}