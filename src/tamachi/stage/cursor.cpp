#pragma once

#include "../head.cpp"


namespace tamachi {
	namespace cursor {

		void show() {
			while ( ShowCursor( true ) < 0 );
		}

		void hide() {
			while ( ShowCursor( false ) >= 0 );
		}

		void reset() {
			show();
		}

	}
}