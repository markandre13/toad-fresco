#include <string>
#include <Fresco/widgets/menu_kit.h>
#include <Fresco/figures/utility.h>

#include "DPicture.hh"

class TToolButton;
class TCTool;

class TToolBox {
	friend TToolButton;
	public:
		TToolBox();
		~TToolBox();
		Glyph_ptr glyph();
		static TCTool* Tool() { return _tool; }
		static TDLayer* Layer() { return _layer; }
		
	private:
		static TCTool* _tool;
		static TDLayer* _layer;
	
    RootTelltale* _root_tt;
    MenuItem* _root;
    ToolBar* _bar;
    MenuManager* _manager;
};
