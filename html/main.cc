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
#include <toad/form.hh>
#include <toad/menubar.hh>

#include "htmlview.hh"
#include "state.hh"

class TMainWindow:
	public TForm
{
	public:
		TMainWindow(TWindow*,const string&);
};

int ToadMain()
{
	TState::font_normal = new TFont(TFont::SERIF, TFont::PLAIN, 12);
	TState::font_h1     = new TFont(TFont::SANS, TFont::BOLD, 16);
	TState::font_h2     = new TFont(TFont::SANS, TFont::BOLD, 14);
	TState::font_h3     = new TFont(TFont::SANS, TFont::BOLD, 12);
	TState::font_tt			= new TFont(TFont::TYPEWRITER, TFont::PLAIN, 12);

	TMainWindow html(NULL, "Nefilim 0.1");
	html.Run();
	
	delete TState::font_normal;
	delete TState::font_h1;
	delete TState::font_h2;
	delete TState::font_h3;
	delete TState::font_tt;
	
	return 0;
}

TMainWindow::TMainWindow(TWindow *p,const string &t):
	TForm(p,t)
{
	SetSize(500,500);
	TMenuBar *mb = new TMenuBar(this, "menubar");
	THTMLView *hv = new THTMLView(this, "htmlview");
	hv->Open("index.html");
	
	Attach(mb, TOP | LEFT | RIGHT);
	Attach(hv, TOP, mb);
	Attach(hv, LEFT | RIGHT | BOTTOM);
	
	TMenuItem *mi;
	mb->BgnPulldown("File");
		mi = mb->AddItem("Open..");
		mi = mb->AddItem("Quit");
		CONNECT(this,closeRequest, mi,mi->sigActivate);
	mb->EndPulldown();
	
	mb->BgnPulldown("Go");
		mi = mb->AddItem("Back");
		CONNECT(hv,hv->Back, mi,mi->sigActivate);
		mi = mb->AddItem("Forward");
		CONNECT(hv,hv->Forward, mi,mi->sigActivate);
		mi = mb->AddItem("Home");
	mb->EndPulldown();
}
