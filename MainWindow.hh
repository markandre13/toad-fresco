#include <Fresco/figures/editor.h>
#include <Fresco/figures/figviewer.h>

class THTMLView;

class TMainWindow: ixx_extends(ViewerImpl)
{
	public:
		TMainWindow();
		MenuItem* get_menu(const char*);
		void home();
    void print();
    void reload();
		void url_text();
		void menuNew();
		void menuHelp();
		
		THTMLView *hv;
		Adjustment_var yadj;
		void vscroll();
};
