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

#ifndef THTMLParser
#define THTMLParser THTMLParser

#include "element.hh"

class THTMLParser
{
	public:
		THTMLParser(const string &value, TElementVector &p);

	protected:
		string input;							// input string
		int eof;									// length of input string
		int pos;									// current position in input string
		
		TElementVector &parsed;		// parsed elements
		TElement *last;						// last added element
		
		PElement element_break;	

		bool inside_command;
		bool inside_html;
		bool inside_head;
		bool inside_body;
		bool inside_pre;
		bool first_of_cmd;
		bool start_cmd;
		
		bool GetToken();
		void TknBgnCommand();
		void TknEndCommand();
		void DistCommandParam(string& lvalue, string& rvalue);
		void DistTextParam(string s);
		void Add(TElement *e);
		bool IsWS();
		bool IsEOL();
		void NextChar();
		void SkipWS();
	public:		
		struct TEBody
			:public TElement
		{
			TEBody();
			void addParam(const string &lvalue, const string &rvalue);
			TRGB bgcolor;
			bool bgcolor_valid;
		};

		TEBody body;
};

#endif
