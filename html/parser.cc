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

#include "parser.hh"
#include "string.hh"

THTMLParser::THTMLParser(const string &value, TElementVector &p)
	:parsed(p)
{
	element_break = new TEBreak();

	input = value+" ";
	eof = value.length();
	pos = 0;
	inside_command = false;
	inside_html    = false;
	inside_head    = false;
	inside_body    = false;
	inside_pre		 = false;
	first_of_cmd   = false;
	start_cmd      = false;

	while(GetToken());
}

bool THTMLParser::GetToken()
{
// cout << "GetToken " << pos << " -> " <<eof << endl;
	again:
	SkipWS();
	if (pos>=eof)
		return false;
	string s;
	if (input[pos]=='<') {
		// skip comments
		//-------------------------------
		if (pos+3<eof && 
				input[pos+1]=='!' && 
				input[pos+2]=='-' &&
				input[pos+3]=='-')
		{
			unsigned p2 = input.find("-->",pos+4);
			if (p2!=string::npos) {
				p2+=3;
				pos=p2;
			}
			goto again;
		}

		// start command
		//-------------------------------
		pos++;
		if (pos<=eof && !IsWS()) {
			TknBgnCommand();
			return true;
		}
		pos--;
	} else if (input[pos]=='>' && inside_command) {
		// end command
		//-------------------------------
		pos++;
		TknEndCommand();
		return true;
	}
	
	// get command or text part
	//---------------------------------
	int l1 = pos;
	if (input[pos]!='<') {		// not a `<' followed by a white space
		if (inside_command) {
			/*
				`BORDER'
				`BORDER=0'
				`BORDER="Hallo"'
				`BORDER = "Hallo"'
			*/
			while(pos<eof && !IsWS() && input[pos]!='=' && input[pos]!='>')
				pos++;
			string lvalue = upper(input.substr(l1,pos-l1));
			
			SkipWS();
			
			string rvalue;
			if (pos<eof && input[pos]=='=') {
				pos++;
				SkipWS();
				if (input[pos]=='\"') {
					pos++;
					l1=pos;
					while(++pos<eof && input[pos]!='\"');
				} else {
					l1=pos;
					while(pos<eof && !IsWS() && input[pos]!='>')
						pos++;
				}
				rvalue = input.substr(l1,pos-l1);
			}
			DistCommandParam(lvalue, rvalue);
			return true;
		} else {
			if (!inside_pre) {
				while(!IsWS() && input[pos]!='<' && pos<eof) {
					pos++;
				}
			} else {
				while(!IsEOL() && input[pos]!='<' && pos<eof) {
					pos++;
				}
			}
		}
	} else {
		pos++;
	}
	
	s = input.substr(l1,pos-l1);
	DistTextParam(s);
	return true;
}
		
void THTMLParser::TknBgnCommand()
{
	inside_command = true;
	first_of_cmd   = true;
	start_cmd      = true;
	last					 = NULL;
}
		
void THTMLParser::TknEndCommand()
{
	inside_command = false;
}
		
void THTMLParser::DistCommandParam(string& lvalue, string& rvalue)
{
	if (first_of_cmd) {
		string s;

		if (lvalue[0]=='/') {
			start_cmd = false;
			s = lvalue.substr(1);
		} else {
			s = lvalue;
		}

//		cout << "cmd:`" << s << "'" << endl;
		if (s=="HTML") {
			inside_html = start_cmd;
		} else if (s=="!DOCTYPE") {	// should check the whole <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
			inside_html=true;
		} else if (s=="BODY") {
			inside_body = start_cmd && inside_html;
			last = &body;
		} else if (s=="BR" && start_cmd) {
			Add(element_break);
		} else if (s=="P") {
			Add(new TEParagraph());
			Add(element_break);
		} else if (s=="HR" && start_cmd) {
			Add(new TEHorizontalRule());
			Add(element_break);
		} else if (s=="PRE") {
			if (start_cmd) {
				inside_pre = true;
				Add(new TEPreformatedBgn());
			} else {
				inside_pre = false;
				Add(new TEPreformatedEnd());
			}
		} else if (s=="H1") {
			if (start_cmd) {
				Add(new TEHeading1Bgn());
			} else {
				Add(new TEHeadingEnd());
			}
		} else if (s=="H2") {
			if (start_cmd) {
				Add(new TEHeading2Bgn());
			} else {
				Add(new TEHeadingEnd());
			}
		} else if (s=="H3") {
			if (start_cmd) {
				Add(new TEHeading3Bgn());
			} else {
				Add(new TEHeadingEnd());
			}
		} else if (s=="UL" || s=="OL") {
			if (start_cmd) {
				Add(new TEUnorderedListBgn());
			} else {
				Add(new TEUnorderedListEnd());
			}
		} else if (s=="LI") {
			if (start_cmd) {
				Add(new TEListItem());
			}
		} else if (s=="A") {
			if (start_cmd) {
				Add(new TEAnchorBgn());
			} else {
				Add(new TEAnchorEnd());
			}
		} else if (s=="IMG") {
			if (start_cmd) {
				Add(new TEImage());
			}
		} else if (s=="TABLE") {
			if (start_cmd) {
				Add(new TETableBgn());
			} else {
				Add(new TETableEnd());
			}
		} else if (s=="TR") {
			if (start_cmd) {
				Add(new TETableRow());
			}
		} else if (s=="TH") {
			if (start_cmd) {
				Add(new TETableHead());
			}
		} else if (s=="TD") {
			if (start_cmd) {
				Add(new TETableData());
			}
		} else {
//cout << "UCD: \"" << s << "\"" << endl;
		}
		first_of_cmd = false;
	} else {
		// System.out.println("extra param: \""+s+"\"");
//cout << "PAR: \"" << s << "\"" << endl;
		if (last) {
			last->addParam(lvalue, rvalue);
		}
	}
}

void THTMLParser::DistTextParam(string s)
{
	if (inside_body) {
//		cout << ":" << s << ":" << endl;
		TEWord *w = new TEWord(s);
		if (inside_pre && IsEOL())
			w->nl_after=true;
#if 0
if (inside_pre) {
	cout << "PRE: \"" << s << "\"" << endl;
	if (IsEOL())
		cout << "PRE: EOL" << endl;
} else {
	cout << "TXT: \"" << s << "\"" << endl;
}
#endif
		Add(w);
	}
}

void THTMLParser::Add(TElement *e)
{
	last = e;
	e->nl_width = !inside_pre;	// don't break lines when line content exceeds
															// the page width in <PRE>...</PRE> areas
	parsed.push_back(e);
}
		
bool THTMLParser::IsWS()
{
	char c = input[pos];
	switch(c) {
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			return true;
	}
	return false;
}

bool THTMLParser::IsEOL()
{
	char c = input[pos];
	switch(c) {
		case '\n':
//		case '\r':
			return true;
	}
	return false;
}
		
void THTMLParser::NextChar()
{
	pos++;
	if (pos>=eof) {
		cout << "stop" << endl;
	}
}
		
void THTMLParser::SkipWS()
{
#ifdef DEBUG_SCAN_WS
	int start = pos;
	while(IsWS() && pos<eof) {
		pos++;
	}
	System.out.println("SkipWs:+\""+input.substring(start,pos)+"\"");
#else
	while (IsWS() && pos<eof) {
		pos++;
	}
#endif
}

THTMLParser::TEBody::TEBody()
{
	bgcolor_valid = false;
}

void THTMLParser::TEBody::addParam(const string &lvalue, const string &rvalue)
{
	if (lvalue=="BGCOLOR") {
		bgcolor_valid = GetColor(rvalue, bgcolor);
#if 0
		if (bgcolor_valid)
			cout << "BGCOLOR:" << (int)rgb.r << "," << (int)rgb.g << "," << (int)rgb.b << endl;
		else
			cout << "BGCOLOR: failed" << endl;
#endif
	}
}