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

#include <toad/scrollbar.hh>

#include "htmlview.hh"
#include "element.hh"
#include "parser.hh"
#include "string.hh"
#include "state.hh"
#include "polygon.hh"
#include "urlstream.hh"

struct TLine {
	int x, y;				// start position of the line (y is the baseline!)
	int eol,bol;		// index range in `parsed' vector
	int ascent, descent;
};

template <class T>
class pvector: public vector<T>
{
		typedef vector<T> super;
	public:
		~pvector() {
			iterator p = begin();
			while(p!=end()) {
				delete *p;
				p++;
			}
		}
		void erase() {
			iterator p = begin();
			while(p!=end()) {
				delete *p;
				p++;
			}
			super::erase(begin(), end());
		}
};

class TLineVector:public pvector<TLine*> {};

TFont *TState::font_normal=NULL, *TState::font_h1, *TState::font_h2, *TState::font_h3, *TState::font_tt;

TState::TState()
{
	SetFont(font_normal);
	SetColor(0,0,0);
}

struct TAnchor {
	TPolygon polygon;
	string href;
};

class TAnchorVector: public pvector<TAnchor*> {} ;

struct TInit {
	int start_idx;
	int end_idx;
	int current_idx;
	int min_width;
	int max_width;
	int y;
	int left;
	int right;
	string current_path;
	TElementVector *parsed;
	TLineVector *lines;
	TAnchorVector *anchors;
};


THTMLView::THTMLView(TWindow *parent, const string &title)
	:TWindow(parent, title), history(20)
{
	SetBackground(TColor::LIGHTGRAY);
	SetSize(512,256);
	SetMouseMoveMessages(TMMM_ALL);

	_vscroll = new TVScrollBar(this, "vscroll");
	CONNECT(this,actVScroll, _vscroll,_vscroll->sigValueChanged);
	
	parsed = new TElementVector();
	lines  = new TLineVector();
	anchors= new TAnchorVector();
}

THTMLView::~THTMLView()
{
	delete parsed;
	delete lines;
	delete anchors;
}

void THTMLView::Open(const string &newurl)
{
	string oldurl = _filename;

	string file = newurl;
	string oldurl2 = oldurl;
	
	if (newurl.find("://")!=string::npos) {
		file = newurl;
	} else {
		file = newurl;
		unsigned n = oldurl2.find("://");
		if (n==string::npos) {
			file="";
		} else {
			n+=3;
			while(oldurl2.size()>n &&
						file.size()>3 && 
						file.substr(0,3)=="../") 
			{
				unsigned l = oldurl2.size()-1;
				if (oldurl2[l]=='/')
					l--;
				unsigned p = oldurl2.rfind("/", l);
				if (p<=n)
					break;
				oldurl2=oldurl2.substr(0,p);
				file = file.substr(3);
			}
			unsigned p = oldurl2.rfind("/");
			oldurl2 = oldurl2.substr(0,p+1);
			if (file[0]=='/') {
				unsigned p = oldurl2.find_first_not_of(" \t");
				const char *str = oldurl2.c_str()+p;
				bool hostname = false;
				if (strncasecmp(str, "http", 4)==0 ||
						strncasecmp(str, "ftp", 3)==0 )
					hostname = true;
				p = oldurl2.find("://");
				p+=3;
				if (hostname)
					p = oldurl2.find("/", p)+1;
				oldurl2=oldurl2.substr(0,p-1);
			}
			file = oldurl2 + file;
		}
	}

	history.Go(file);
	_Open(file);
}

void SetURLText(const string &t);

void THTMLView::_Open(const string &filename)
{
	string fn = filename;

	iurlstream url(fn);
	string value;
	while(url) {
		char buffer[1025];
		url.read(buffer, 1024);
		value.append(buffer,url.gcount());
	}

	// set path of current document
	//----------------------------------------------------------
	_filename = fn;
	SetURLText(_filename);

	SetValue(value);
}

void THTMLView::SetValue(const string &html)
{
	parsed->erase();
	lines->erase();
	anchors->erase();
	_current = NULL;

	THTMLParser hp(html, *parsed);
	if (hp.body.bgcolor_valid)
		SetBackground(hp.body.bgcolor);
	else
		SetBackground(192,192,192);
	SetOrigin(0,0);
	_vscroll->SetValue(0);
	Init();
	Invalidate();
}

void THTMLView::Back()
{
	history.Prev();
	_Open(history.Current());
}

void THTMLView::Forward()
{
	history.Next();
	_Open(history.Current());
}

void THTMLView::mouseLDown(int x,int y,unsigned)
{
	y += _vscroll->Value();

	TAnchor *a = NULL;
	unsigned n = anchors->size();
	for(unsigned i=0; i<n; i++) {
		a = (*anchors)[i];
		if (a->polygon.IsInside(x,y)) {
			Open(a->href);
			return;
		}
	}
}

void THTMLView::mouseMove(int x, int y, unsigned)
{
	if (_current && _current->polygon.IsInside(x,y))
		return;

	y += _vscroll->Value();

	TAnchor *a = NULL;
	unsigned n = anchors->size();
	for(unsigned i=0; i<n; i++) {
		a = (*anchors)[i];
		if (a==_current) 
			continue;
		if (a->polygon.IsInside(x,y)) {
			_current = a;
			return;
		}
	}
	_current = NULL;
}

void THTMLView::resize()
{
	Init();
	
	_vscroll->SetShape(
		Width()-_vscroll->FixedSize()+1,-1,
		_vscroll->FixedSize(), Height()+2);
}

// void THTMLView::actVScroll(TIntManipulator *v)		Segmentation Fault!!!

void THTMLView::actVScroll(TScrollBar*){
	int ny = -_vscroll->Value();			// new position
	ScrollTo(0,ny);
}

void anchor_check0(TAnchorVector *a, int dx, int dy);

void THTMLView::Init()
{
	lines->erase();
	TInit init;
	init.parsed     	= parsed;
	init.lines				= lines;
	init.anchors			= anchors;
	init.start_idx 		= 0;
	init.end_idx			= parsed->size();
	init.y						= 8;
	init.left					= 8;
	init.right				= Width() - 8 - _vscroll->Width()+1;
	init.current_path = _filename.substr(0, _filename.rfind("/")+1);
	Init(init,1);
	Init(init,2);

	_vscroll->SetRange(0,init.y+8+8);
	_vscroll->SetVisible(Height());

	// should put the anchors stuff here or in a own method instead of
	// doing it in Init(...) (now it's in paint)

	// find hyperlink areas
	//-------------------------------------------------------------------------
	// currently i'm using a Polygon but this will become a Region in the
	// future when problems appear with tables and the like

	anchor_check0(init.anchors, _dx, _dy);
}
	
void THTMLView::Init(TInit &init, int stage)
{
	TStateInit state;
	state.parsed				= init.parsed;
	state.y 						= init.y;
	state.ascent 				= 8;
	state.descent				= 8;
	state.left					= init.left;
	state.right					= init.right;
	state.x							= state.left;
	state.end_idx 			= init.end_idx;
	state.current_path	= init.current_path;

	TLine *line = NULL;

	// stage 1
	//-------------------------------------------------------------------------
	int width_till_newline = 0;
	if (stage==1) {
		init.min_width=0;
		init.max_width=0;
	}

	// stage 2
	//-------------------------------------------------------------------------
	int max_ascent = 0, max_descent = 0;	// line content
	int max_top    = 0, max_bottom  = 0;  // extra for paragraph
	if (stage==2) {
		line = new TLine();
		line->x   = state.x;
		line->bol = line->eol = init.start_idx;
		init.lines->erase();
		init.lines->push_back(line);
	}

	for(int i=init.start_idx; i<init.end_idx;) {
		TElement *e = (*init.parsed)[i];
		state.Reset();
		if (stage==1 || stage==2) {
			state.current_idx = i;
			switch(stage) {
				case 1:
					e->init(state, 1);
					break;
				case 2:
					e->init(state, 2);
					e->init(state, 3);
			}
		}
		state.font_changed = false;
		state.color_changed = false;
		for(int k=0; k<3; k++) {
			bool newline = false;
			switch(k) {
				case 0:
					newline = e->nl_before;
					if (stage==1) {
						if (newline) { 
							if (init.max_width<width_till_newline)
								init.max_width = width_till_newline;
							width_till_newline = 0;
						}
					}
					if (e->nl_width && state.x+e->width > state.right)
						newline=true;
					break;
				case 1:
					if (stage==2) {
						if (max_ascent<state.element_ascent)
							max_ascent = state.element_ascent;
						if (max_descent<state.element_descent)
							max_descent = state.element_descent;
						if (max_top<state.par_top)
							max_top = state.par_top;
						if (max_bottom<state.par_bottom)
							max_bottom = state.par_bottom;
					}

					e->afterPaint(state);
					state.x += e->width;
					i				+= e->size;
					if (stage==2) {
						line->eol=i;
					}
					if (stage==1) {
						if (state.min_width>=0) {
							if (init.min_width < state.min_width)
								init.min_width=state.min_width;
						} else {
							if (init.min_width < e->width)
								init.min_width=e->width;
						}
						if (state.max_width>=0)
							width_till_newline += state.max_width;
						else
							width_till_newline += e->width;
						if (e->space)
							width_till_newline += state.GetSpaceWidth();
					}
					if (state.x!=state.left && e->space)
						state.x += state.GetSpaceWidth();
					if (e->nl_width && state.x >= state.right)
						newline = true;
					break;
				case 2:
					newline = e->nl_after;
					break;
			}
			if (newline) {
				int end_of_last;
				// store values of current line
				//-----------------------------
				if (stage==2) {
					state.y += max_ascent + max_top;
					line->y = state.y;
					line->ascent = max_ascent;
					line->descent = max_descent;
					end_of_last = line->eol;
				}
				// move to the next line
				//----------------------
				state.x = state.left;
				if (stage==2) {
					state.y += max_descent + max_bottom;
					line = new TLine();
					init.lines->push_back(line);
				// prepare values for next line
				//-----------------------------
					max_ascent = 0;
					max_descent = 0;
					max_top = 0;
					max_bottom = 0;
					line->x = state.x;
					line->bol = line->eol = end_of_last;
				}
			}
		}
	}

	if (init.max_width<width_till_newline)
		init.max_width = width_till_newline;
	if (stage!=2) {
		return;
	}

	state.y += max_ascent /*+ max_descent */;
	if (stage==2) {
		line->y = state.y;
		line->eol = init.end_idx;
		line->ascent = max_ascent;
		line->descent = max_descent;
	}
	//System.out.println("currently there are "+init.lines.size()+" lines");

	init.y = state.y;			// return value for stage 2
}

bool create_anchors = false;
TAnchorVector *anchors;
TAnchor *current_anchor = NULL;
int x1,x2,x3,x4, y1,y2,y3,y4;
int cdx, cdy;

void anchor_check0(TAnchorVector *a, int dx, int dy)
{
	create_anchors = true;
	anchors = a;
	anchors->erase();
	cdx = dx;
	cdy = dy;

	if (current_anchor)
		delete current_anchor;		
	current_anchor = NULL;

	/*
		The meaning of x1..x4, y1..y4:
	
		y1		        +---------------------------------+
									|                                 |
		y2			+-----+                                 |
						|                                       |
		y3			|                      +----------------+
						|                      |
		y4			+----------------------+
			      x1    x2               x3               x4
	*/
}

void anchor_check1(TElement *e, int x, int y, const TLine *line)
{
	if (!create_anchors)
		return;

	TEAnchorBgn *a = DYNAMIC_CAST(TEAnchorBgn, e);
	if (a) {
		if (a->href.size()!=0) {
// cout << "found anchor start at:" << x << "," << y << "-" << line->ascent << endl;
			if (current_anchor) {
				// issue a warning message here!
				delete current_anchor;
			}
			current_anchor = new TAnchor;
			current_anchor->href = a->href;
			x1 = x2 = x3 = x4 = x;
			y1 = y-line->ascent;
			y2 = y3 = y4 = y+line->descent;
		}
	}

	if (current_anchor!=NULL) {
		if (x1>x)
			x1=x;
	}
}

void anchor_check2(TElement *e, int x, int y, const TLine *line)
{
	if (!create_anchors)
		return;

	if (current_anchor!=NULL) {
		if (x4<x)
			x4=x;
	}

	if (DYNAMIC_CAST(TEAnchorEnd, e) && current_anchor) {
		x3 = x;
		y3 = y-line->ascent;
		y4 = y+line->descent;
		
		y1-=cdy;
		y2-=cdy;
		y3-=cdy;
		y4-=cdy;
		
		current_anchor->polygon.AddPoint(x2,y1);
		current_anchor->polygon.AddPoint(x4,y1);
		current_anchor->polygon.AddPoint(x4,y3);
		current_anchor->polygon.AddPoint(x3,y3);
		current_anchor->polygon.AddPoint(x3,y4);
		current_anchor->polygon.AddPoint(x1,y4);
		current_anchor->polygon.AddPoint(x1,y2);
		current_anchor->polygon.AddPoint(x2,y2);
		anchors->push_back(current_anchor);
// cout << "found anchor end at:" << x << "," << y << "+" << line->descent << endl;
		current_anchor = NULL;
	}
}

void THTMLView::paint()
{
	TInit init;
	init.parsed 		= parsed;
	init.lines			= lines;
	init.anchors		= anchors;
	init.start_idx 	= 0;
	init.end_idx		= parsed->size();
	init.y					= 0;			// translation!!!
	init.left				= 8;
	init.right			= Width() - 8;
	TPen pen(this);
	
	paint(pen, init);
	create_anchors = false;

#if 0
	// paint anchor boundarys
	//------------------------
	if (init.anchors) {
		unsigned n = init.anchors->size();
		pen.SetColor(TColor::LIGHTRED);
		for(unsigned i=0; i<n; i++) {
			TAnchor *a = (*init.anchors)[i];
			pen.DrawPolygon(a->polygon.begin(), a->polygon.size());
		}
	}
#endif
}
	
void THTMLView::paint(TPen &pen, TInit &init)
{
	TStateInit state;
	state.parsed	= init.parsed;
	state.y 			= init.y;			// needed ?
	state.ascent 	= 8;					
	state.descent	= 8;
	state.left		= init.left;
	state.right		= init.right;
	state.x				= state.left;
	state.end_idx = init.end_idx;
	
	pen.SetFont(state.font);

	int n = init.lines->size();	
	for(int i=0; i<n; i++) {
		TLine *line = (*init.lines)[i];
		state.x = line->x;
		state.y = line->y + init.y;
		for(int j=line->bol; j<line->eol; ) {
			TElement *e = (*init.parsed)[j];
anchor_check1(e, state.x+pen._dx, state.y+pen._dy, line);
			state.font_changed = false;
			state.color_changed = false;
			pen.Translate(state.x,state.y);
			e->paint(pen, state);
			pen.Translate(-state.x,-state.y);
			e->afterPaint(state);
			state.x += e->width;
anchor_check2(e, state.x+pen._dx, state.y+pen._dy, line);
			j       += e->size;
			if (state.font_changed)
				pen.SetFont(state.font);
			if (state.color_changed)
				pen.SetColor(state.color);

			// special stuff for space to be removed...
			if (state.x!=state.left && e->space)
				state.x += state.GetSpaceWidth();
		}
	}
	
}

// TTable
//---------------------------------------------------------------------------
struct TETableBgn::TField
{
	TField() {
		start_idx = -1;
		data = NULL;
	}
	int start_idx, end_idx;
	TLineVector lines;
	TETableData *data;
};

TETableBgn::TETableBgn()
{
	cellspacing = 2;
	cellpadding = 5;
	border = 0;
	row = NULL;
	col = NULL;
	field = NULL;
	pwidth = -1;
	cwidth = -1;
}

TETableBgn::~TETableBgn()
{
	if (row) delete[] row;
	if (col) delete[] col;
	if (field) delete[] field;
}

void TETableBgn::addParam(const string& lvalue, const string& rvalue)
{
	if (lvalue=="WIDTH") {
		pwidth=GetPercent(rvalue);
		if (pwidth==-1)
			cwidth=GetInteger(rvalue,-1);
	} else if (lvalue=="CELLPADDING") {
		cellpadding=GetInteger(rvalue);
	} else if (lvalue=="CELLSPACING") {
		cellspacing = GetInteger(rvalue);
	} else if (lvalue=="BORDER") {
		border = GetInteger(rvalue, 1);
	} else if (lvalue=="ALIGN") {
		align = GetAlign(rvalue);
	}
}
		
void TETableBgn::init(TStateInit& state, int stage)
{
	nl_before = true;
	nl_after  = true;
	state.par_top = 6;
	state.par_bottom = 6;
	
	switch(stage) {
		case 1: {
			// calculate number of rows and columns and how many elements
			// belong to the table
			//--------------------------------------------------------------
			rows = 0;
			cols = 0;
			int cols_in_row = 0;		// the maximum will be stored in `cols'
			int counter = 0;				// to skip inner tables

			start_idx = state.current_idx;
			for(end_idx=state.current_idx+1; end_idx<state.end_idx; end_idx++) {
				TElement *e = (*state.parsed)[end_idx];
				if (DYNAMIC_CAST(TETableBgn, e)) {
					counter++;
				} else
				if (DYNAMIC_CAST(TETableEnd, e)) {
					if (counter==0) {
						state.current_idx = end_idx;
						break;
					}
					counter--;
				} else
				if (counter==0 && DYNAMIC_CAST(TETableRow, e)) {
					rows++;
					cols_in_row = 0;
				} else
				if (counter==0 && 
						(DYNAMIC_CAST(TETableHead,e) || DYNAMIC_CAST(TETableData, e))) 
				{
					if (rows==0)
						rows++;
					cols_in_row++;
					if (cols_in_row>cols)
						cols = cols_in_row;
				}
			}
			size = end_idx-start_idx+1;
	
			// setup field array: create fields and set start_idx & end_idx
			//--------------------------------------------------------------
if(field!=NULL) delete[] field;
			field = new TField[cols*rows];
			int x,y;
	
			x = y = -1;
			counter = 0;
			for(int i=start_idx+1; i<end_idx; i++) {
				TElement *e = (*state.parsed)[i];
				if (DYNAMIC_CAST(TETableBgn,e)) {
					counter++;
				} else
				if (DYNAMIC_CAST(TETableEnd,e)) {
					counter--;
				} else
				if (!counter && DYNAMIC_CAST(TETableRow,e)) {
					if (x>=0 && y>=0)
						field[x+y*cols].end_idx = i;
					y++;
				x=-1;
			} else
				if (!counter && (DYNAMIC_CAST(TETableHead,e) || DYNAMIC_CAST(TETableData,e))) {
					if (x>=0 && y>=0)
						field[x+y*cols].end_idx = i;
					if (y<0)
						y++;
					x++;
					field[x+y*cols].start_idx = i;
					field[x+y*cols].data = static_cast<TETableData*>(e);
				}
			}
			if (x>=0 && y>=0)
				field[x+y*cols].end_idx = end_idx;
		
			// setup field, row and column array: calculate min & max width
			//--------------------------------------------------------------
if (col!=NULL) delete[] col;
			col = new TCol[cols];
			for(x=0; x<cols; x++) {
				col[x].min_width = 0;
				col[x].max_width = 0;
			}

			TInit init;
			init.current_path = state.current_path;
			init.parsed = state.parsed;
			for(y=0; y<rows; y++) {
				for(x=0; x<cols; x++) {
					TField *f = &field[x+y*cols];
		//					System.out.print("["+x+","+y+"]:");
					if (f->start_idx>=0) {
						init.start_idx	= f->start_idx;
						init.end_idx		= f->end_idx;
						init.left = 0; init.right = 10000; // dummy values during stage 1
						THTMLView::Init(init,1);
						if (f->data && f->data->cwidth>init.min_width) {
							init.min_width = f->data->cwidth;
						}
						init.min_width+=cellpadding<<1;
						init.max_width+=cellpadding<<1;
						if (col[x].min_width<init.min_width)
							col[x].min_width = init.min_width;
						if (col[x].max_width<init.max_width)
							col[x].max_width = init.max_width;
#if 0
cout<< " idx:"<<f->start_idx<<","<<f->end_idx
		<<" min:"<<init.min_width
		<<" max_width:"<<init.max_width<<endl;
#endif
					}
				}
			}

			// calculate table minimal and maximal width
			//-------------------------------------------
			table_min_width = table_max_width = 0;
			for(x=0; x<cols; x++) {
				table_min_width+=col[x].min_width;
				table_max_width+=col[x].max_width;
			}

			state.min_width = table_min_width;
			state.max_width = table_max_width;
			} break; // end of stage 1
			
		case 2: {
			int x;
		
			// calculate table width
			//-------------------------------------------
			if (cwidth>=0) {
				width=cwidth;
			} else {
				width = state.right - state.left;
			}
//cout << "table: min:"<<table_min_width<<" max:"<<table_max_width<<" available:"<<width<<endl;
			if (width < table_min_width)
				width = table_min_width;
			else if (width > table_max_width)
				width = table_max_width;
			if (pwidth>0) {
				int w2 = (state.right-state.left)*pwidth/100;
				if (width<w2)
					width = w2;
			}

			// calculate column width
			//-------------------------------------------
			if (width==table_max_width) {
				for(x=0; x<cols; x++) {
					col[x].width=col[x].max_width;
				}
			} else if (width==table_min_width) {
				for(x=0; x<cols; x++) {
					col[x].width=col[x].min_width;
				}
			} else {
				for(x=0; x<cols; x++) {
					col[x].width=col[x].min_width + (int)
					(
						((double)width-(double)table_min_width) *
						((double)col[x].max_width/(double)table_max_width)
					);
				}
			}
		}	break; // end of stage 2

		case 3: {
			int x,y;
			// calculate field height & line layout
			//-------------------------------------------
if (row) delete[] row;
			row = new TRow[rows];
	
			int yp = cellpadding;
			for(y=0; y<rows; y++) {
				int xp = 0;
				row[y].max_height = 0;
				for(x=0; x<cols; x++) {
					TField *f = &field[x+y*cols];
					if (f->start_idx>=0) {
						TInit init;
						init.current_path = state.current_path;
						init.parsed			= state.parsed;
						init.lines			= &f->lines;
						init.anchors		= NULL; /*state.anchors*/
						init.start_idx	= f->start_idx;
						init.end_idx		= f->end_idx;
						init.y					= yp;
						init.left 			= xp+cellpadding;
						init.right			= xp+col[x].width-cellpadding;
						THTMLView::Init(init,2);
						xp+=col[x].width;
						int height = init.y-yp;
						if (row[y].max_height < height)
							row[y].max_height = height;
					}
				}
				row[y].max_height += (cellpadding<<1);
				yp+=row[y].max_height;
			}
			height = yp - cellpadding /*- state.y*/;
			state.element_ascent = height;
		} break; // end of stage 3
	}	
}	

void TETableBgn::paint(TPen& pen, TState& state)
{
	int xp, yp;
	if (border!=0) {
		pen.DrawRectangle(0,-height,width+1,height+1);
		xp = 0;
		for(int x=0; x<cols-1; x++) {
			xp+=col[x].width;
			pen.DrawLine(xp,-height,xp,0);
		}

		yp = -height;
		for(int y=0; y<rows-1; y++) {
			yp+=row[y].max_height;
			pen.DrawLine(0,yp,width,yp);
		}
	}
	
	yp=-height;		
	for(int y=0; y<rows; y++) {
		xp=0;
		for(int x=0; x<cols; x++) {
			TField *f = &field[x+y*cols];
			if (f->data && f->data->bgcolor_valid) {
				pen.SetColor(f->data->bgcolor);
				pen.FillRectangle(xp+1,yp+1,col[x].width-1,row[y].max_height-1);
				pen.SetColor(TColor::BLACK);
			}
			if (f->start_idx>=0) {
				TInit init;
				init.parsed			= state.parsed;
				init.lines			= &f->lines;
				init.start_idx 	= f->start_idx;
				init.end_idx		= f->end_idx;
				init.anchors		= NULL;
				init.y					= -height;
				init.left				= xp;
				init.right			= xp+col[x].width;
				THTMLView::paint(pen, init);
			}
			xp+=col[x].width;
		}
		yp+=row[y].max_height;
	}
#if 0
	pen.SetColor(255,0,0);
	pen.DrawLine(0,-2,table_max_width,-2);
	pen.SetColor(0,255,0);
	pen.DrawLine(0,-1,table_min_width,-1);
	pen.SetColor(0,0,0);
#endif
}

void TETableBgn::afterPaint(TState& state)
{
	state.x += width;
}

THistory::THistory(unsigned s)
{
	size = s;
	pos = s;
	start = end = 0;
	buffer = new string[s];
}

THistory::~THistory()
{
	delete[] buffer;
}

void THistory::Go(const string &name)
{
//cout << "go to " << name << endl;
	unsigned op = pos;
	pos++;
	if (pos>=size)
		pos=0;
	end=pos;
	buffer[pos]=name;
	if (pos==start && op!=size)
		start=pos+1;
	if (start>=size)
		start=0;
}

void THistory::Next()
{
//	cout << "next" << endl;
	if (pos==size || pos==end)
		return;
	unsigned np = pos + 1;
	if (np>=size)
		np=0;
	if (np!=start)
		pos=np;
}

void THistory::Prev()
{
//	cout << "prev" << endl;
	if (pos==start || pos==size)
		return;
	if (pos==0)
		pos=size-1;
	else
		pos--;
}

void THistory::Print()
{
	for(unsigned i=0; i<size; i++) {
		cout << i << ":" << buffer[i] << endl;
	}
}
