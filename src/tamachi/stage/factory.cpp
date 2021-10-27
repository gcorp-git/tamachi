#pragma once

#include "stage.cpp"


namespace tamachi {
	namespace stage {
		namespace factory {

			bool _is_inited = false;

			uint64_t _IDS = 0;

			std::unordered_map<uint64_t, Stage*> _stages_map = {};
			std::unordered_map<HWND, Stage*> _windows_map = {};

			void _on_open( uint64_t id, HWND window );
			void _on_close( uint64_t id, HWND window );
			LRESULT CALLBACK _callback( HWND window, UINT uMsg, WPARAM wParam, LPARAM lParam );

			void init( HINSTANCE instance ) {
				if ( _is_inited ) return;

				_is_inited = true;

				_instance = instance;

				_wc.style = CS_HREDRAW | CS_VREDRAW;
				_wc.lpszClassName = "TAMACHI_WINDOW_CLASS";
				_wc.lpfnWndProc = _callback;

				RegisterClass( &_wc );
			}

			Stage* create() {
				if ( !_is_inited ) return nullptr;

				auto id = ++_IDS;
				auto _stage = new Stage( id, _on_open, _on_close );

				_stages_map.insert_or_assign( id, _stage );

				return _stage;
			}

			void destroy( Stage* _stage ) {
				if ( !_is_inited ) return;
				if ( !_stage ) return;

				_stage->close();

				auto found = _stages_map.find( _stage->id() );

				if ( found != _stages_map.end() ) _stages_map.erase( found );

				delete _stage;
			}

			void _on_open( uint64_t id, HWND window ) {
				if ( !_is_inited ) return;

				auto found = _stages_map.find( id );

				if ( found == _stages_map.end() ) return;

				auto _stage = _stages_map[ id ];

				_windows_map.insert_or_assign( window, _stage );
			}

			void _on_close( uint64_t id, HWND window ) {
				if ( !_is_inited ) return;

				auto found = _windows_map.find( window );

				if ( found == _windows_map.end() ) return;

				_windows_map.erase( found );
			}

			LRESULT CALLBACK _callback( HWND window, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
				auto found = _windows_map.find( window );

				if ( found != _windows_map.end() ) {
					return _windows_map[ window ]->callback( window, uMsg, wParam, lParam );
				}

				return DefWindowProc( window, uMsg, wParam, lParam );
			}

		}
	}
}