#include <Fresco/widgets/menu_kit.h>

class TToolBox;

class TToolButton
{
		MenuItem_ptr _widget;
	public:
		TToolButton();
		TToolButton(TToolBox*, const char*);
		void SetLabel(const char*);
		void SetTelltale(Telltale_ptr root);
		operator MenuItem_ptr()
		{
			return _widget;
		}
};
