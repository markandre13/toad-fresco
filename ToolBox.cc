#include "help.hh"
#include "ToolBox.hh"
#include "CTool.hh"
#include <Fresco/figures/utility.h>
#include <iostream>

#include <Fresco/figures/manipulators.h>
#include <Fresco/figures/tools.h>

#include "action.hh"

TCTool* TToolBox::_tool = NULL;

TToolBox::TToolBox()
{
	_root_tt = new RootTelltale;
	_root = menukit->submenu_item();
	_bar = menukit->tool_bar(_root);
	_manager = menukit->menu_manager(_root);
	_manager->root_menu(_bar);
	
	_tool = new TCToolRect();
}

TToolBox::~TToolBox()
{
	CORBA::release(_root_tt);
	CORBA::release(_root);
	CORBA::release(_manager);
	CORBA::release(Viewer_ptr(_bar));
}

Glyph_ptr TToolBox::glyph()
{
//	_root->append(menukit->separator_item());
	_bar->need_resize();
	return _bar;
}
