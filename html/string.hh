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

inline string upper(const string &str)
{
	string result(str);
	string::iterator p = result.begin(), e = result.end();
	while(p!=e) {
		*p = toupper(*p);
		p++;
	}
	return result;
}

inline bool starts_with(const string &s1, const string &s2)
{
	return s1.compare(s2,0,s2.size())==0;
}

inline string trim(const string &s)
{
	return s;
}
