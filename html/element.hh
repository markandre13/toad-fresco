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

#ifndef TElement
#define TElement TElement

#include <toad/toad.hh>
#include <toad/pointer.hh>
#include <vector>
#include <string>

class TState;
class TStateInit;

// #define HAS_RTTI

#ifdef HAS_RTTI
	#include <typeinfo>
	#define DYNAMIC_CAST(T,V) dynamic_cast<T*>(V)
	#define BGN_STRUCT0(NC,ID) struct NC {
	#define BGN_STRUCT(NC,SC,ID) struct NC: public SC {
	#define END_STRUCT };
#else
	enum ERTTI {
		EELEMENT=1, EWORD, EBREAK, EPARAGRAPH, EPREFORMATEDBGN, EPREFORMATEDEND,
		EHEADING1BGN, EHEADING2BGN, EHEADING3BGN, EHEADINGEND, EHORIZONTALRULE,
		EUNORDEREDLISTBGN, EUNORDEREDLISTEND, ELISTITEM, EANCHORBGN, EANCHOREND,
		EIMAGE, ETABLEBGN, ETABLEEND, ETABLEROW, ETABLEDATA, ETABLEHEAD
	};
	#define DYNAMIC_CAST(T,V) ((T*)pseudo_cast(T::_ID, V))
	struct _RTTI {
		virtual bool _rtti(int id) { return false; }
		static const int _ID=0;
	};
	inline _RTTI* pseudo_cast(int id, _RTTI *o) {
		return o->_rtti(id) ? o : 0;
	}
	
	#define RTTI_BASE :public _RTTI
	
	#define BGN_STRUCT0(NC, SC, ID) \
		struct NC: public SC, public _RTTI { \
			bool _rtti(int id) { return id==_ID ? true : _RTTI::_rtti(id); } \
			static const ERTTI _ID = ID;
			
	#define BGN_STRUCT(NC,SC,ID) \
		struct NC: public SC { \
			bool _rtti(int id) { return id==_ID ? true : SC::_rtti(id); } \
			static const ERTTI _ID = ID;
			
	#define END_STRUCT };
#endif

BGN_STRUCT0(TElement, TSmartReference, EELEMENT)
		TElement();
		virtual ~TElement() {}
		int width;				// must be set to final value after init stage 1
		unsigned size;		// must be set to final value after init stage 1
											// the default value is `1' and tells you how
											// many elements this element measures
		bool space:1;			// must be set to final value after init stage 1
											// in some situations this flag will also be set
											// by the parser
		bool nl_before:1;	// new line before element, default=false and
											// must be set after init()
		bool nl_width:1;	// new line when line exceeds right border, default=true
											// and must be set after init()
		bool nl_after:1;	// new line after element  (default=true) 
											// abd must be set after init()
		virtual void addParam(const string& lvalue, const string& rvalue) { }
		virtual void init(TStateInit& state, int stage) { }
		virtual void paint(TPen& pen, TState& state) { }
		virtual void afterPaint(TState& state) { }

		static int GetPercent(const string &s);
		static int GetInteger(const string &s, int default_value=0);
		enum EAlign { ALIGN_NONE, ALIGN_LEFT, ALIGN_MIDDLE, ALIGN_RIGHT };
		static EAlign GetAlign(const string &s);
		
		static bool GetColor(const string &s, TRGB&);	
END_STRUCT

typedef GSmartPointer<TElement> PElement;

class TElementVector
	:public vector<PElement>
{
		typedef vector<PElement> super;
	public:
		void erase() {
			super::erase(begin(), end());
		}
};

BGN_STRUCT(TEWord, TElement, EWORD)
	string word;

	TEWord(const string& s);
	void init(TStateInit& state, int stage);
	void paint(TPen& pen, TState& state);
END_STRUCT

BGN_STRUCT(TEBreak, TElement, EBREAK)
	TEBreak();
END_STRUCT

BGN_STRUCT(TEParagraph, TElement, EPARAGRAPH)
	void init(TStateInit& state, int stage);
END_STRUCT

BGN_STRUCT(TEPreformatedBgn, TElement, EPREFORMATEDBGN)
	void init(TStateInit& state, int stage);
	void afterPaint(TState& state);
END_STRUCT 

BGN_STRUCT(TEPreformatedEnd, TElement, EPREFORMATEDEND)
	void init(TStateInit& state, int stage);
	void afterPaint(TState& state);
END_STRUCT 
	
BGN_STRUCT(TEHeading1Bgn, TElement, EHEADING1BGN)
	void init(TStateInit& state, int stage);
	void afterPaint(TState& state);
END_STRUCT 

BGN_STRUCT(TEHeading2Bgn, TEHeading1Bgn, EHEADING2BGN)
	void afterPaint(TState& state);
END_STRUCT

BGN_STRUCT(TEHeading3Bgn, TEHeading1Bgn, EHEADING3BGN)
	void afterPaint(TState& state);
END_STRUCT

BGN_STRUCT(TEHeadingEnd, TElement, EHEADINGEND)
	void init(TStateInit& state, int stage);
	void afterPaint(TState& state);
END_STRUCT

BGN_STRUCT(TEHorizontalRule, TElement, EHORIZONTALRULE)
	void init(TStateInit& state, int stage);
	void paint(TPen& pen, TState& state);
END_STRUCT

BGN_STRUCT(TEUnorderedListBgn, TElement, EUNORDEREDLISTBGN)
	TEUnorderedListBgn();
	void afterPaint(TState& state);
END_STRUCT

BGN_STRUCT(TEUnorderedListEnd, TElement, EUNORDEREDLISTEND)
	TEUnorderedListEnd();
	void afterPaint(TState& state);
END_STRUCT

BGN_STRUCT(TEListItem, TElement, ELISTITEM)
	TEListItem();
	void paint(TPen& pen, TState& state);
END_STRUCT


BGN_STRUCT(TEAnchorBgn, TElement, EANCHORBGN)
	string href;
	string name;
	void addParam(const string& lvalue, const string& rvalue);
	void afterPaint(TState& state);
END_STRUCT

BGN_STRUCT(TEAnchorEnd, TElement, EANCHOREND)
	void afterPaint(TState& state);
END_STRUCT

class TBitmap;

BGN_STRUCT(TEImage, TElement, EIMAGE)
	int _w;
	int _h;
	string src;
	string alt;
	int border;
	TEImage();
	~TEImage();
	void addParam(const string& lvalue, const string& rvalue);
	void init(TStateInit& state, int stage);
	void paint(TPen& pen, TState& state);
	
	TBitmap *bitmap;
END_STRUCT
	
BGN_STRUCT(TETableBgn, TElement, ETABLEBGN)
	int start_idx, end_idx;
	int height, width;				// table size in pixels
	int rows, cols;						// table size in fields
	int table_min_width;
	int table_max_width;
	EAlign align;
		
	int border;								// controls frame width around table
	int cellspacing;					// spacing between cells
	int cellpadding;					// spacing within cells to cell content
	int pwidth;								// width in percent (-1=none)
	int cwidth;								// const width (-1=none)

	struct TField;
	TField *field;
	
	struct TRow
	{
		int max_height;			// of all fields in a row
	};
	TRow *row;

	struct TCol
	{
		int min_width, max_width;			// of all fields in a column
		int width;
	};
	TCol *col;
		
	TETableBgn();
	~TETableBgn();
  void addParam(const string& lvalue, const string& rvalue);
  void init(TStateInit& state, int stage);
	void paint(TPen& pen, TState& state);
	void afterPaint(TState& state);
END_STRUCT

BGN_STRUCT(TETableEnd, TElement, ETABLEEND)
END_STRUCT

BGN_STRUCT(TETableRow, TElement, ETABLEROW)
END_STRUCT

BGN_STRUCT(TETableData, TElement, ETABLEDATA)
	TETableData();
	void addParam(const string &lvalue, const string &rvalue);
	int cwidth;
	TRGB bgcolor;
	bool bgcolor_valid;
END_STRUCT

BGN_STRUCT(TETableHead, TETableData, ETABLEHEAD)
END_STRUCT

#endif
