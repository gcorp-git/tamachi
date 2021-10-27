#pragma once

#include "../canvas/canvas.cpp"
#include "../utils/fps.cpp"
#include "../utils/storage.cpp"
#include "../utils/listeners.cpp"
#include "factories/scene.factory.cpp"
#include "camera.cpp"
#include "input.cpp"


namespace tamachi {
	namespace stage {

		HINSTANCE _instance = NULL;
		WNDCLASS _wc = {};

	}
}