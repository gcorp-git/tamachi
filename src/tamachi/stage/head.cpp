#pragma once

#include "../head.cpp"


namespace tamachi {
	namespace stage {

		bool _is_changed = false;
		bool _is_updated = false;

		enum FLUSH_MODE {
			MODE_DEFAULT,
			MODE_STRETCH,
			MODE_CENTER,
		};

	}
}