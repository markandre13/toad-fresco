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

#ifndef __TOAD_TYPES_HH
#define __TOAD_TYPES_HH

typedef unsigned long ulong;
typedef unsigned char byte;

// from 'X11/Xlib.h'
struct TPoint {
	TPoint() {x=y=0;}
	TPoint(int a, int b):x(a),y(b){}
  short x,y;
  void Set(short a,short b) {
  	x=a;y=b;
  }
};

struct TRect {
	int x,y,w,h;
	TRect(){Set(0,0,0,0);};
	TRect(int x,int y,int w,int h){Set(x,y,w,h);}
	void operator =(const TRect &r){x=r.x;y=r.y;w=r.w;h=r.h;}
	void Set(int a,int b,int c,int d){x=a;y=b;w=c;h=d;}
	bool IsInside(int px,int py) {
		return (x<=px && px<x+w && y<=py && py<y+h);
	}
	void Adjust() {
		if (w<0) { x+=w-1; w=-w; }
		if (h<0) { y+=h-1; h=-h; }
	}
};

#endif
   