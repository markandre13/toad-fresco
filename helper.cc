/*
 * This is to hide the parts of Fresco which insult the eye.
 *
 */

#include "help.hh"

#include <Fresco/figures/utility.h>

FrescoContext *fcontext;

FigureKit* figurekit;
DrawingKit* drawingkit;
LayoutKit* layoutkit;
WidgetKit* widgetkit;
MenuKit*   menukit;
ImageKit*	 imagekit;
Screen_var screen;
Display_var display;

void InitFresco(char *name, int argc, char** argv)
{
	fcontext = FrescoLib::init_default_context(name,argc,argv);
	display = fcontext->display_ptr();
	screen = display->default_screen();

	figurekit = KitFactory::instance()->figures();
	drawingkit= KitFactory::instance()->drawings();
	layoutkit = KitFactory::instance()->layouts();
	widgetkit = KitFactory::instance()->widgets();
	menukit   = KitFactory::instance()->menus();
	imagekit  = Fresco_resolve_object(ImageKit, "images");
}

void RunViewer(Viewer_ptr viewer)
{
	Window_var window = screen->application(viewer);
	Viewer_var(window->main_viewer())->request_focus(viewer, false);
	window->map();
	display->run(true);
}