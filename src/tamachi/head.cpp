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

	struct Point {
		double x, y;

		Point( double _x=0, double _y=0 ) {
			x = _x;
			y = _y;
		}
	};

}