/*
 * TOAD -- A Simple and Powerful C++ GUI Toolkit for X-Windows
 * Copyright (C) 1996-98 by Mark-André Hopf
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,   
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public 
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "polygon.hh"

bool TPolygon::IsInside(int x, int y)
{
	int hits = 0;
	int ySave = 0;
	int i = 0;
	int npoints = size();
	while (i < npoints && (*this)[i].y == y) {
		i++;
	}
			
	for (int n = 0; n < npoints; n++) {
		int j = (i + 1) % npoints;
		int x1 = (*this)[i].x, y1 = (*this)[i].y;
		int x2 = (*this)[j].x, y2 = (*this)[j].y;
				
		int dx = x2 - x1;
		int dy = y2 - y1;
				
		if (dy != 0) {
			int rx = x - x1;
			int ry = y - y1;
			
			if (y2 == y && x2 >= x) {
				ySave = y1;
			}
			if (y1 == y && x1 >= x) {
				if ((ySave > y) != (y2 > y)) {
					hits--;
				}
			}
			
			double s = (double)ry / (double)dy;
			if (s >= 0.0 && s <= 1.0 && (s * dx) >= rx) {
				hits++;
			}
		}
		i=j;
	}
	return (hits % 2) != 0;
}
