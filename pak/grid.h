/*
 *   Citybuilding Mappers - create minimaps from citybuilding game files
 *   Copyright (C) 2007, 2008  Bianca van Schaik
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef GRID_H
#define GRID_H

/**
* Template class for the grids used in the main program
*/
template <class T>
class Grid {
	public:
		/**
		* Constructs a new grid of size x by y
		* @param x width of the grid
		* @param y height of the grid
		*/
		Grid(int x, int y) {
			grid = new T*[y];
			for (int i = 0; i < y; i++) {
				grid[i] = new T[x];
			}
			this->x = x;
			this->y = y;
		}
		
		~Grid() {
			for (int i = 0; i < y; i++) {
				delete[] grid[i];
			}
			delete[] grid;
		}
		
		/**
		* Sets the value of (x, y) to item. This function checks
		* the boundaries for invalid values.
		* @param x X position
		* @param y Y position
		* @param item Item to put on (x, y)
		*/
		void set(int x, int y, T item) {
			if (x < 0 || y < 0 || x >= this->x || y >= this->y) {
				return;
			}
			grid[y][x] = item;
		}
		
		/**
		* Returns the value at (x, y)
		* @param x X position
		* @param y Y position
		* @return T item at position (x, y)
		*/
		T get(int x, int y) const {
			if (x < 0 || y < 0 || x >= this->x || y >= this->y) {
				return (T)0;
			}
			return grid[y][x];
		}
		
	private:
		int x, y;
		T ** grid;
};

#endif /* GRID_H */
