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
#include <cmath>


namespace tamachi {

	HINSTANCE _hInstance;

	template<typename T>
	struct Point {
		T x, y;

		Point( T _x=0, T _y=0 ) {
			x = _x;
			y = _y;
		}
	};

}