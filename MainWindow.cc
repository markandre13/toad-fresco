#include <stdio.h>
#include <stdlib.h> 

typedef unsigned long ulong;

#include <iostream.h>

#include <Fresco/base/fresco.h>
#include <Fresco/base/proxyobs.h>
#include <Fresco/defs/layouts.h>
#include <Fresco/defs/widgets.h>
#include <Fresco/base/viewers.h>
#include <Fresco/defs/figures.h>

#include "help.hh"
#include "MainWindow.hh"
#include "MenuBar.hh"
#include "ToolBox.hh"
#include "ToolButton.hh"
#include "CTool.hh"
#include "action.hh"

#include <Fresco/figures/utility.h>
#include <Fresco/figures/commands.h>

#include "html/htmlview.hh"

inline int rint(double a) { return (int)(0.5+a); }

Glyph_ptr LoadImage(const char* filename); 

FieldEditor_var text_ed;
FieldSubject_var text_fsubj;
FrescoObject_var text_callback;
Glyph_var titel;

const Glyph_ptr label(const char* str, FigureKit_ptr figurekit) {
    FigureStyle_var figstyle = figurekit->default_style();
    CharString_var char_string = FrescoLib::string_ref(str);
    return figurekit->label(figstyle, char_string);
}

Glyph_ptr label_and_field(
    const char* str, FieldEditor_ptr fe
) {
    Glyph_ptr wrapped_fe = widgetkit->inset_frame(fe);
    Glyph_ptr hbox = layoutkit->hbox();
    hbox->append(label(str, figurekit));
    hbox->append(wrapped_fe);
    return widgetkit->outset_frame(hbox);
}

FrescoObject_var url_callback;

TMainWindow::TMainWindow()
{
#if 0
  Glyph_ptr label_and_field(
  		 const char* str, FieldEditor_ptr ed
  	);
#endif  	
	hv = new THTMLView(NULL, "htmlview");
  try {
    hv->Open("file://index.html");
  } catch (exception &e) {
    cerr << e.what() << endl;
  }
  
	TMenuBar *mb = new TMenuBar();
	mb->BgnPulldown("File");
//	mb->AddItem("Open", new ActionCallback<TMainWindow>(this,&TMainWindow::menuNew));
//	mb->AddItem("Save", new ActionCallback<TMainWindow>(this,&TMainWindow::menuNew));
//	mb->AddItem("Save As..", new ActionCallback<TMainWindow>(this,&TMainWindow::menuNew));
//	mb->AddSeparator();
	mb->AddItem("Quit", new QuitCmd());
	mb->BgnPulldown("Go");
	mb->AddItem("Back", 	 new ActionCallback<THTMLView>(hv, &THTMLView::Back));
	mb->AddItem("Forward", new ActionCallback<THTMLView>(hv, &THTMLView::Forward));
	mb->BgnPulldown("Help");
	mb->AddItem("Help", 	 new ActionCallback<TMainWindow>(this, &TMainWindow::menuHelp));

	yadj = widgetkit->bounded_float(0.0,1.0,1.0);
	Glyph_var sb = widgetkit->scroll_bar(Y_axis, yadj);
	new UpdateCallback<TMainWindow>(this, &TMainWindow::vscroll, yadj);
	
  // arrange the widgets
  //---------------------
  Glyph_var img0 = LoadImage("pfeil02.gif");
  Glyph_var img1 = LoadImage("pfeil0.gif");
  Glyph_var img2 = LoadImage("house.gif");
  Glyph_var img3 = LoadImage("druckergelbgrau.gif");
  Glyph_var img4 = LoadImage("reload.gif"); 

  Glyph_var pb = widgetkit->push_button(
  	img0,
		new ActionCallback<THTMLView>(hv, &THTMLView::Back));
  Glyph_var pb1 = widgetkit->push_button(
    img1,
    new ActionCallback<THTMLView>(hv, &THTMLView::Forward));
	
	Glyph_var pb2 = widgetkit->push_button(
		img2,
		new ActionCallback<TMainWindow>(this,&TMainWindow::home)	 
	);
	Glyph_var pb3 = widgetkit->push_button(
		img3,
		new ActionCallback<TMainWindow>(this,&TMainWindow::print)
	);
	Glyph_var pb4 = widgetkit->push_button(
	  img4,
	  new ActionCallback<TMainWindow>(this,&TMainWindow::reload)	
	);
	
	text_fsubj = widgetkit ->field_subject();
	text_fsubj ->replace_all(
		CharString_var(FrescoLib::string_ref("file://index.html"))
	);
	text_ed = widgetkit->field_editor(text_fsubj,20,250);													
	
  url_callback = new ProxyObserver( text_fsubj, new ActionCallback<TMainWindow>(this,&TMainWindow::url_text));
                                                  
  titel = label_and_field(" URL: ", text_ed);

		
	Glyph_var hbox = layoutkit->hbox();
	hbox->append(Glyph_var(layoutkit->valign(hv->GetViewer(), Coord(0.0))));
	hbox->append(Glyph_var(layoutkit->valign(sb, Coord(0.0))));
  
  Glyph_var hbox2 = layoutkit->hbox();
  hbox2->append(pb);
  hbox2->append(pb1);
  hbox2->append(pb2);
  hbox2->append(pb3);
  hbox2->append(pb4);
  hbox2->append(layoutkit->hfil());
  
  Glyph_var hbox3 = layoutkit->hbox();
  hbox3->append(titel);

  Glyph_var interior = layoutkit->vbox();
  interior->append(mb->glyph());
	interior->append(hbox2);
	interior->append(hbox3);
  interior->append(hbox);

  body(
  	layoutkit->back(
			interior,
      widgetkit->filler()
    )
  );
}

void TMainWindow::home()
{
	text_fsubj->replace_all(FrescoLib::string_ref("file://index.html"));
}

void TMainWindow::print()
{
	printf("print\n");
}

void TMainWindow::reload()
{
	printf("reload\n");
}

static bool dont_open = false;

void SetURLText(const string &t)
{
	if (!text_fsubj)
		return;
	dont_open = true;
	text_fsubj->replace_all(FrescoLib::string_ref(t.c_str()));
	dont_open = false;
}

void TMainWindow::url_text()
{
	static bool recursive = false;
	if (recursive || dont_open)
		return;
	recursive = true;
	try {
		hv->Open(text_fsubj->get_all() );
	} catch (exception &e) {
		cerr << e.what() << endl;
	}

	// TEST:
	recursive = false;
}
	
void TMainWindow::vscroll()
{
	Adjustment::Settings s;
	yadj->get_settings(s);
	float y = (1.0 - s.cur_lower) * hv->_vscroll->max;
	hv->ScrollTo(0, rint(-y));
}

Glyph_ptr LoadImage(const char* filename)
{
  ImageKit  *imagekit = Fresco_resolve_object(ImageKit,"images");
  Raster_var bmp = imagekit->open(filename);
  if (is_nil(bmp)) {                         
		printf("Konnte %s nicht laden\n", filename);
    exit(1);                                    
  }
  
  Raster::Info info;
  bmp->raster_info(info); 

 Float scale = Float(72) / screen->dpi();
 bmp->scale(scale);  
 
 return figurekit->image(bmp);
} 
                 
void TMainWindow::menuNew()
{
	cout << "menuNew" << endl;
}

void TMainWindow::menuHelp()
{
  try {
    hv->Open("memory://about.html");
  } catch (exception &e) {
    cerr << e.what() << endl;
  }
}

// required method with unknown function...
MenuItem* TMainWindow::get_menu(const char*)
{
	return nil;
}
