#include <Fresco/figures/utility.h>
#include <Fresco/figures/manipulators.h>
#include <Fresco/figures/tools.h>
#include <iostream>

#include "help.hh"
#include "ToolBox.hh"
#include "ToolButton.hh"

#include "action.hh"

TToolButton::TToolButton()
{
	_widget = menukit->leaf_item();
	Telltale_var t = new TelltaleImpl(
		TelltaleImpl::enabled_bit | TelltaleImpl::choosable_bit
	);
	_widget->mark_state(t);
}

TToolButton::TToolButton(TToolBox *box, const char* txt)
{
	_widget = menukit->leaf_item();
	Telltale_var t = new TelltaleImpl(
		TelltaleImpl::enabled_bit | TelltaleImpl::choosable_bit
	);
	_widget->mark_state(t);
	t->current(box->_root_tt);
	SetLabel(txt);
	box->_root->append(_widget);
}

void TToolButton::SetLabel(const char *text)
{
	_widget->label_string(text);
}

void TToolButton::SetTelltale(Telltale_ptr root)
{
	_widget->mark_state()->current(root);
}
