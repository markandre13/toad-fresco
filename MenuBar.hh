#include <string>
#include <Fresco/widgets/menu_kit.h>

class TMenuBar {
	public:
		TMenuBar();
		~TMenuBar();
		void BgnPulldown(const string &);
		void AddItem(const string&, Action_ptr);
		void AddSeparator();
		
		Glyph_ptr glyph();
		
	private:
		MenuItem_ptr _root;
		MenuBar_var _bar;
		MenuManager_var _mgr;
		
		MenuItem_ptr _pd;
};
