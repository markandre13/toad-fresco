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

#ifndef TState
#define TState TState

class TState
{
	public:
		TState();

		int x;				// current x-Position
		int y;				// current y baseline position
		int ascent;		// ascent of current line
		int descent;	// descent of current line
		int left;			// left border of current line
		int right;		// right border of current line

		// font stuff
		//------------------------------------------------------
		TFont *font;
		
		static TFont *font_normal, *font_h1, *font_h2, *font_h3, *font_tt;

		bool font_changed;
		

		void SetFont(TFont *f) {
			font = f;
			font_changed = true;
		}

		int GetSpaceWidth()	{
			return font->TextWidth(" ");
		}

		// color stuff
		//------------------------------------------------------
		TColor color;
		bool color_changed;
		void SetColor(int r,int g, int b) {
			color.Set(r,g,b);
			color_changed = true;
		}
		
		// parser stuff
		//------------------------------------------------------
		TElementVector *parsed;
		int current_idx;
		int end_idx;
};

// this `TState' is only used during `Init()' and has some additional
// attributes
//-------------------------------------------------------------------
struct TStateInit :
	public TState
{
		int element_ascent;
		int element_descent;
		int par_top;
		int par_bottom;
		int max_width;
		int min_width;
		
		string current_path;		// directory of current document

		void Reset() {
			element_ascent = 0;
			element_descent = 0;
			par_top = 0;
			par_bottom = 0;
			max_width = -1;
			min_width = -1;
		}

		int GetWidth(const string& s) const {
			return font->TextWidth(s);
		}
		int Ascent() const {
			return font->Ascent();
		}
		int Descent() const {
			return font->Descent();
		}
};

#endif
