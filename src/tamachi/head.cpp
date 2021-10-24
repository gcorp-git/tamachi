#pragma once

#include <windows.h>
#include <windowsx.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <chrono>
#include <vector>
#include <unordered_map>


namespace tamachi {

	bool _is_changed = false;
	bool _is_updated = false;

	HINSTANCE _hInstance;

}