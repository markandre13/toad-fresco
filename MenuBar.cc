#include <Fresco/figures/utility.h>
#include <iostream>

#include "help.hh"
#include "MenuBar.hh"

TMenuBar::TMenuBar()
{
	_root = menukit->submenu_item();
		_bar = menukit->menu_bar(_root);				// Menübalken
			_mgr = menukit->menu_manager(_root);	// Manager
			_mgr->root_menu(_bar);                // Top
}

TMenuBar::~TMenuBar()
{
	cout << "destroying TMenuBar" << endl;
}

void TMenuBar::BgnPulldown(const string &name)
{
	_pd = menu_item_string(name.c_str(), name[0]);
	_root->append(_pd);
}

void TMenuBar::AddItem(const string &name, Action_ptr action)
{
	_pd->append(MenuItem_var(button_item_string(name.c_str(),name[0], action)));
}

void TMenuBar::AddSeparator()
{
	_pd->append(menukit->separator_item());
}

Glyph_ptr TMenuBar::glyph()
{
	_root->append(menukit->separator_item());
	_bar->need_resize();
	return _bar;
}
