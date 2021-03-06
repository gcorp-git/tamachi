#pragma once


namespace tamachi {
	namespace grid {

		enum BORDERS {
			TOP_BORDER = 0b0001,
			RIGHT_BORDER = 0b0010,
			BOTTOM_BORDER = 0b0100,
			LEFT_BORDER = 0b1000,
		};

		struct Cell {
			int64_t x, y;
			uint32_t width, height;
		};

		class Grid {
		public:

			Grid(
				uint32_t cw, uint32_t ch,
				uint8_t borders=0,
				int64_t top=0, int64_t right=0, int64_t bottom=0, int64_t left=0
			) {
				_cw = cw;
				_ch = ch;
				_borders = borders;
				_top = top;
				_right = right;
				_bottom = bottom;
				_left = left;
			}

			bool find( Point p, Cell* cell=nullptr ) {
				if ( !_is_point_in_borders( p ) ) return false;

				if ( cell ) {
					cell->x = static_cast<int64_t>( p.x / static_cast<double>( _cw ) );
					cell->y = static_cast<int64_t>( p.y / static_cast<double>( _ch ) );

					cell->width = _cw;
					cell->height = _ch;
				}

				return true;
			}

			void ray( Point from, Point to, double limit, std::function<bool(Cell, Point)> handler ) {
				double dcw = static_cast<double>( _cw );
				double dch = static_cast<double>( _ch );

				Point _from = { from.x / dcw, from.y / dch };
				Point _to = { to.x / dcw, to.y / dch };

				double dx = _to.x - _from.x;
				double dy = _to.y - _from.y;
				double mag = std::sqrt( dx * dx + dy * dy );

				Point dir = { dx / mag, dy / mag };

				Point step_size = {
					std::sqrt( 1 + ( dir.y / dir.x ) * ( dir.y / dir.x ) ),
					std::sqrt( 1 + ( dir.x / dir.y ) * ( dir.x / dir.y ) )
				};

				Point ray_length;
				Point step;

				Point cell_pos = { _from.x, _from.y };
				
				if ( dir.x < 0 ) {
					step.x = -1;
					ray_length.x = step_size.x * ( _from.x - cell_pos.x );
				} else {
					step.x = 1;
					ray_length.x = step_size.x * ( ( cell_pos.x + 1 ) - _from.x );
				}

				if ( dir.y < 0 ) {
					step.y = -1;
					ray_length.y = step_size.y * ( _from.y - cell_pos.y );
				} else {
					step.y = 1;
					ray_length.y = step_size.y * ( ( cell_pos.y + 1 ) - _from.y );
				}

				double distance = 0;
				
				Point intersection;

				Cell cell;
				cell.width = _cw;
				cell.height = _ch;

				while ( true ) {
					cell.x = static_cast<int64_t>( cell_pos.x );
					cell.y = static_cast<int64_t>( cell_pos.y );

					if ( !_is_cell_in_borders( cell ) ) return;

					intersection.x = _cw * ( _from.x + dir.x * distance );
					intersection.y = _ch * ( _from.y + dir.y * distance );

					auto dx = dcw * dir.x * distance;
					auto dy = dch * dir.y * distance;

					if ( std::sqrt( dx * dx + dy * dy ) > limit ) return;
					if ( handler( cell, intersection ) ) return;

					if ( ray_length.x < ray_length.y ) {
						distance = ray_length.x;
						ray_length.x += step_size.x;
						cell_pos.x += step.x;
					} else {
						distance = ray_length.y;
						ray_length.y += step_size.y;
						cell_pos.y += step.y;
					}
				}
			}

			uint32_t get_cell_width() { return _cw; }
			uint32_t get_cell_height() { return _ch; }

		private:
			uint32_t _cw, _ch;
			uint8_t _borders;
			int64_t _top, _right, _bottom, _left;

			bool _is_point_in_borders( Point p ) {
				if ( !_borders ) return true;

				if ( ( _borders & TOP_BORDER ) && ( p.y < _top * _ch ) ) return false;
				if ( ( _borders & RIGHT_BORDER ) && ( p.x > _right * _cw ) ) return false;
				if ( ( _borders & BOTTOM_BORDER ) && ( p.y > _bottom * _ch ) ) return false;
				if ( ( _borders & LEFT_BORDER ) && ( p.x < _left * _cw ) ) return false;

				return true;
			}

			bool _is_cell_in_borders( Cell cell ) {
				if ( !_borders ) return true;

				if ( ( _borders & TOP_BORDER ) && ( cell.y < _top ) ) return false;
				if ( ( _borders & RIGHT_BORDER ) && ( cell.x > _right ) ) return false;
				if ( ( _borders & BOTTOM_BORDER ) && ( cell.y > _bottom ) ) return false;
				if ( ( _borders & LEFT_BORDER ) && ( cell.x < _left ) ) return false;

				return true;
			}

		};

	}
}