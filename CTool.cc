#include <iostream>

#include "CTool.hh"

#include "help.hh"
#include <Fresco/base/painters.h>

// CTool
//---------------------------------------------------------------------------
void TCTool::mouseLDown(TDPoint&){}
void TCTool::mouseMDown(TDPoint&){}
void TCTool::mouseRDown(TDPoint&){}
void TCTool::mouseLUp(TDPoint&){}
void TCTool::mouseMUp(TDPoint&){}
void TCTool::mouseRUp(TDPoint&){}
void TCTool::mouseMove(TDPoint&){}
void TCTool::paint(Painter*){}

// Rectangle
//---------------------------------------------------------------------------
TCToolRect::TCToolRect()
{
	_state = 1;
}

void TCToolRect::mouseLDown(TDPoint &p)
{
	_start = _end = p;
	_state = 2;
	cout << "mouseLDown: " << p.x << ", " << p.y << endl;
}

void TCToolRect::mouseMove(TDPoint &p)
{
	if (_state==2)
	{
		cout << "mouseMove: " << p.x << ", " << p.y << endl;
		_end = p;
	}
}

void TCToolRect::mouseLUp(TDPoint &p)
{
	_state = 1;
	cout << "mouseLUp: " << p.x << ", " << p.y << endl;
}

void TCToolRect::paint(Painter* p)
{
	if (_state==2) {
		p->rect(_start.x, _start.y, _end.x, _end.y);
		p->stroke();
	}
}