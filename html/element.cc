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

#include <toad/toad.hh>
#include <toad/pointer.hh>
#include <vector>
#include <cstdio>
#include <ctype.h>

#include "element.hh"
#include "string.hh"
#include "state.hh"

TElement::TElement()
{
	width = 0;
	space = false;
	size  = 1;
	nl_before=false;
	nl_width=true;
	nl_after=false;
}


TEWord::TEWord(const string& s)
{
#include "lex_special_char.cc"

#define DM(A)

DM(cout << "in: \"" << s << "\"" << endl;)
	space = true;
	
	unsigned pos=0;
	unsigned lpos;
	unsigned eos=s.size();
	word="";
	while(true) {
continue1:
		lpos = pos;
		pos = s.find("&",lpos);
		if (pos==string::npos)
			pos=eos;
DM(cout << "next substring1 is \"" << s.substr(lpos,pos-lpos) << "\"" << endl;)
		word+=s.substr(lpos,pos-lpos);
		lpos = pos;
		if (pos>=eos)
			break;
DM(cout << "found &" << endl;)
		pos++;
		if (pos>=eos)
			break;

		if (s[pos]=='#') {
			pos++;
			if (pos>=eos)
				break;
			unsigned pos2 = s.find_first_not_of("0123456789",pos);
			DM(cout << "found number #" << s.substr(pos,pos2-pos) << "#" << endl;)
			char result='?';
			if (pos2-pos-1>0) {
				int r = atoi(s.substr(pos,pos2-pos).c_str());
				if (r>=32 && r<=255)
					result = r;
				if (s[pos2]==';')
					pos2++;
			}
			pos=pos2;
			word+=result;
			goto continue1;
		}
			
		// search the tree stored in `table'
		//--------------------------------------
		unsigned current_table = 0;
		while(true) {
			current_table = table[current_table][s[pos]-'A'];
			pos++;
			if (current_table>=156)
				break;
			if (current_table==0 || pos>=eos) {
				pos=lpos+1;
				DM(cout << "failed on special char" << endl;)
DM(cout << "next substring2 is \"" << s.substr(lpos,pos-lpos) << "\"" << endl;)
				word+=s.substr(lpos,pos-lpos);
				goto continue1;
			}
		}
		
		// convert last table entry into a char
		//--------------------------------------
		char result;
		static const char out_of_order[]="\"&<>";
		if (current_table<160)
			result = out_of_order[current_table-156];
		else
			result = (char)current_table;
		
		// compare the rest
		//------------------
		const char *p = rest_strings[rest_pointer[current_table-156]];
		unsigned sl = strlen(p);
		DM(cout << "end: "<<s[pos+sl] <<" rest: "<< s.substr(pos,sl) << "=?" << p << ": " << result << endl;)
		if (s[pos+sl]==';' && s.substr(pos,sl)==p) {
			pos+=sl+1;
			word+=result;
		} else {
DM(cout << "next substring3 is \"" << s.substr(lpos,pos-lpos) << "\"" << endl;)
				word+=s.substr(lpos,pos-lpos);
				continue;
		}
	}
	if (pos>lpos)
		word+=s.substr(lpos,pos-lpos);
DM(cout << "out: \"" << word << "\"" << endl;)
#undef DM
}

void TEWord::init(TStateInit& state, int /*stage*/)
{
	width = state.GetWidth(word);
	state.element_ascent = state.Ascent();
	state.element_descent= state.Descent();
}

void TEWord::paint(TPen& pen, TState& state)
{
	pen.DrawString(0,-pen.Ascent(),word);
}

TEBreak::TEBreak()
{
	nl_before = true;
}

void TEParagraph::init(TStateInit& state, int /*stage*/)
{
	nl_before = true;
	state.par_top = 4;
	state.par_bottom = 8;
}

void TEPreformatedBgn::init(TStateInit& state, int /*stage*/)
{
	nl_before = true;
	state.par_top = 4;
}
	
void TEPreformatedBgn::afterPaint(TState& state)
{
	state.SetFont(state.font_tt);
}

void TEPreformatedEnd::init(TStateInit& state, int /*stage*/)
{
	nl_after = true;
	state.par_bottom = 4;
}
	
void TEPreformatedEnd::afterPaint(TState& state)
{
	state.SetFont(state.font_normal);
}
	
void TEHeading1Bgn::init(TStateInit& state, int /*stage*/)
{
	nl_before = true;
	state.par_top = 4;
}
	
void TEHeading1Bgn::afterPaint(TState& state)
{
	state.SetFont(state.font_h1);
}

void TEHeading2Bgn::afterPaint(TState& state)
{
	state.SetFont(state.font_h2);
}

void TEHeading3Bgn::afterPaint(TState& state)
{
	state.SetFont(state.font_h3);
}

void TEHeadingEnd::init(TStateInit& state, int /*stage*/)
{
	nl_before = true;
	state.par_bottom = 8;
}

void TEHeadingEnd::afterPaint(TState& state)
{
	state.SetFont(state.font_normal);
}

void TEHorizontalRule::init(TStateInit& state, int stage)
{
	if (stage<3) {
		nl_before = true;
		nl_after  = true;
		width = 0;
		state.element_ascent = 2;
		state.element_descent = 2;
	} else {
		width = state.right-state.left;
	}
}
		
void TEHorizontalRule::paint(TPen& pen, TState& state)
{
	pen.DrawLine(0,0,width,0);
}

TEUnorderedListBgn::TEUnorderedListBgn()
{
	nl_before = true;
}
	
void TEUnorderedListBgn::afterPaint(TState& state)
{
	state.left += 16;
}

TEUnorderedListEnd::TEUnorderedListEnd()
{
	nl_before = true;
}

void TEUnorderedListEnd::afterPaint(TState& state)
{
	state.left -= 16;
}

TEListItem::TEListItem()
{
	nl_before = true;
}
	
void TEListItem::paint(TPen& pen, TState& state)
{
	pen.FillCircle(-12,-8,6,6);
}


void TEAnchorBgn::addParam(const string &lvalue, const string &rvalue)
{
	if (lvalue=="HREF") {
		href=rvalue;
	} else if (lvalue=="NAME") {
		name=rvalue;
	}
}

void TEAnchorBgn::afterPaint(TState& state)
{
	if (href.size()!=0)
		state.SetColor(0,0,255);
}

void TEAnchorEnd::afterPaint(TState& state)
{
	state.SetColor(0,0,0);
}

// <IMG>
//---------------------------------------------------------------------------
TEImage::TEImage()
{
	_w = -1;
	_h = -1;
	border = 1;
	bitmap = NULL;
}

TEImage::~TEImage()
{
	if (bitmap) {
		delete bitmap;
	}
}

void TEImage::addParam(const string &lvalue, const string &rvalue)
{
	if (lvalue=="WIDTH") {
		_w = GetInteger(rvalue,-1);
	} else if (lvalue=="HEIGHT") {
		_h = GetInteger(rvalue,-1);
	} if (lvalue=="SRC") {
		src = rvalue;
	} if (lvalue=="ALT") {
		alt = rvalue;
	} if (lvalue=="BORDER") {
		border = GetInteger(rvalue, 1);
	}
}

void TEImage::init(TStateInit& state, int /*stage*/)
{
	if (src.size()!=0 && bitmap==NULL) {
		string file = state.current_path + src;
		bitmap = new TBitmap();
		if (bitmap->Load(file)) {
			if (_w==-1)
				_w = bitmap->width;
			if (_h==-1)
				_h = bitmap->height;
		} else {
			delete bitmap;
			bitmap = NULL;
		}
	}
	if (_w==-1)
		_w = 32;
	if (_h==-1)
		_h = 32;
	width = _w + (border<<1);
	state.element_ascent = _h + (border<<1);
}

void TEImage::paint(TPen& pen, TState& state)
{
	pen.DrawRectangle(0,-_h,_w + (border<<1),_h + (border<<1));
	if (bitmap) {
		pen.DrawBitmap(0+border,-_h+border, bitmap);
	}
}

TETableData::TETableData()
{
	cwidth = -1;
	bgcolor_valid = false;
}

void TETableData::addParam(const string &lvalue, const string &rvalue)
{
	if (lvalue=="WIDTH") {
		cwidth = GetInteger(rvalue,-1);
	} else if (lvalue=="BGCOLOR") {
		bgcolor_valid = GetColor(rvalue, bgcolor);
	}
}

// static utility methods
//---------------------------------------------------------------------------
int TElement::GetPercent(const string &s) {
	if (s[s.length()-1]=='%') {
		return atoi(s.substr(0,s.length()-1).c_str());
	}
	return -1;
}

int TElement::GetInteger(const string &s, int d)
{
	if (s.size()==0)
		return d;
	return atoi(s.c_str());
}

TElement::EAlign TElement::GetAlign(const string &s)
{
	if (s=="LEFT")
		return ALIGN_LEFT;
	if (s=="MIDDLE")
		return ALIGN_MIDDLE;
	if (s=="RIGHT")
		return ALIGN_RIGHT;
	return ALIGN_NONE;
}

bool TElement::GetColor(const string &s, TRGB &c)
{
#if 1
	unsigned p=0;
	string u = upper(s);
	if (u=="BLACK"  ){	c.Set(  0,  0,  0);	return true; }
	if (u=="MAROON" ){	c.Set(128,  0,  0);	return true; }
	if (u=="GREEN"  ){	c.Set(  0,128,  0);	return true; }
	if (u=="OLIVE"  ){	c.Set(128,128,  0);	return true; }
	if (u=="NAVY"   ){	c.Set(  0,  0,128);	return true; }
	if (u=="PURPLE" ){	c.Set(128,  0,128);	return true; }
	if (u=="TEAL"   ){	c.Set(  0,128,128);	return true; }
	if (u=="GRAY"   ){	c.Set(128,128,128);	return true; }
	if (u=="SILVER" ){	c.Set(192,192,192);	return true; }
	if (u=="RED"    ){	c.Set(255,  0,  0);	return true; }
	if (u=="LIME"   ){	c.Set(  0,255,  0);	return true; }
	if (u=="YELLOW" ){	c.Set(255,255,  0);	return true; }
	if (u=="BLUE"   ){	c.Set(  0,  0,255);	return true; }
	if (u=="FUCHSIA"){	c.Set(255,  0,255);	return true; }
	if (u=="AQUA"   ){	c.Set(  0,255,255);	return true; }
	if (u=="WHITE"  ){	c.Set(255,255,255);	return true; }
	
	if (s[p]=='#') {
		p++;
	}
	c.Set(0,0,0);
	unsigned v = 0;
	for(unsigned i=0; i<6 && p<s.size(); i++) {
		byte b = s[p++];
//cout << "CHAR: " << b << endl;
		if (b>='0' && b<='9')
			b-='0';
		if (b>='a' && b<='f')
			b-='a'-10;
		if (b>='A' && b<='F')
			b-='A'-10;
		if (b==s[p])
			return i!=0;
//cout << "DIGIT:" << (int)b << endl;
		v+=b;
		if (!(i&1))
			v<<=4;
//cout << "VALUE:" << v << endl;
		switch(i>>1) {
			case 0:	c.r = v; break;
			case 1:	c.g = v; break;
			case 2:	c.b = v; break;
		}
		if (i&1)
			v=0;
	}
	return true;
#else
	return false;
#endif 
}
