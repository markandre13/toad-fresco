#include <Fresco/base/fresco.h>
#include <iostream>

void InitFresco(char *name, int argc, char** argv);
void RunViewer(Viewer_ptr viewer);

class FigureKit;
class LayoutKit;
class WidgetKit;
class DrawingKit;
class MenuKit;
class ImageKit;

extern FrescoContext *fcontext;
extern FigureKit* figurekit;
extern DrawingKit* drawingkit;
extern LayoutKit* layoutkit;
extern WidgetKit* widgetkit;
extern MenuKit*   menukit;
extern ImageKit*  imagekit;
extern Screen_var screen;
extern Display_var display;
