#include <toad/toad.hh>
#include <html/urlstream.hh>
#include "../help.hh"

#include <Fresco/base/viewers.h>
#include <Fresco/figures/utility.h>
#include <Fresco/figures/commands.h>

#include <unistd.h>
#include <fcntl.h>

inline int rint(double a)
{
	return (int)(0.5+a);
}

// TFont
//---------------------------------------------------------------------------
class TFont::TData
{
	public:
		Font_var f;
		Font::Info info;
};

TFont::TFont()
{
	_data = new TData();

//	CharString_var font_family = FrescoLib::string_ref("helvetica");
	CharString_var font_family = FrescoLib::string_ref("times");

	// bold italic underline
	CharString_var font_style = FrescoLib::string_ref("");
	Coord point_size = 12;
	
	_data->f = drawingkit->find_font_match(
		font_family, 
		font_style, 
		point_size);
		
	_data->f->font_info(_data->info);
}

TFont::TFont(EFamily family, EStyle style, int size)
{
	static const char* sfamily[4] = {
		"helvetica",
		"times",
		"",
		"courier"
	};
	static const char* sstyle[7] = {
		"",
		"bold",
		"italic",
		"bold italic",
		"italic",
		"bold italic",
		"underline"
	};

	_data = new TData();

	CharString_var font_family = FrescoLib::string_ref(sfamily[family]);
	CharString_var font_style = FrescoLib::string_ref(sstyle[style]);
	Coord point_size = size;
	
	_data->f = drawingkit->find_font_match(
		font_family, 
		font_style, 
		point_size);
		
	_data->f->font_info(_data->info);
}

TFont::~TFont()
{
	delete _data;
}

int TFont::TextWidth(const string &s)
{
	CharString_var str;
	if (s!=" ")	
		str = FrescoLib::string_ref(s.c_str());
	else
		str = FrescoLib::string_ref("  ");
	Font::Info info;
	_data->f->string_info(str, info);
	return rint(info.width);
}

int TFont::Ascent()
{
	return rint(_data->info.font_ascent);
}

int TFont::Descent()
{
	return rint(_data->info.font_descent);
}

// TBitmap
//---------------------------------------------------------------------------
class TBitmap::TData
{
	public:
		Raster_var bmp;
};

TBitmap::TBitmap()
{
	_data = NULL;
	width = 0;
	height = 0;
}

TBitmap::~TBitmap()
{
	if (_data)
		delete _data;
}

bool TBitmap::Load(const string &fn)
{
	if (_data) {
		delete _data;
		_data = NULL;
	}
	
	iurlstream url(fn);
	int fd = open("/tmp/.fresco_local_image", O_WRONLY|O_CREAT|O_TRUNC, 0600);
	while(url) {
		char buffer[1025];
		url.read(buffer, 1024);
		write(fd, buffer, url.gcount());
	}
	close(fd);
	
	Raster_var bmp = imagekit->open("/tmp/.fresco_local_image");
	unlink("/tmp/.fresco_local_image");
	if (is_nil(bmp)) {
		return false;
	}
	_data = new TData();
	_data->bmp = bmp;
	
	Raster::Info info;
	bmp->raster_info(info);
	width = info.columns;
	height = info.rows;
	
#if 0
	Screen_var screen = display->default_screen();
	Float scale = Float(72) / screen->dpi();
	bmp->scale(scale);
	g = figurekit->image(bmp);
#endif
	return true;
}

// TPen
//---------------------------------------------------------------------------
class TPen::TData
{
	public:
		Painter_var p;
		int h;
};

TPen::TPen(TWindow *wnd)
{
	_wnd = wnd;
	_dx = _wnd->_dx;
	_dy = _wnd->_dy;
	
	_data = new TData();
	_data->p = wnd->_traversal->current_painter();
	Vertex lower, upper, origin;
	wnd->_traversal->bounds(lower, upper, origin);
	_data->p->push_clipping();
	_data->p->clip_rect(lower.x, lower.y, upper.x, upper.y);
	_data->h = (int)upper.y;
	
	Color_var r = drawingkit->color_rgb(0,0,0);
	_data->p->current_color(r);
	
	_font = new TFont();
	_data->p->current_font(_font->_data->f);
}

TPen::~TPen()
{
	_data->p->pop_clipping();
	delete _data;
/*
	if (_font)
		delete _font;
*/
}

void TPen::DrawString(int x,int y,const string &s)
{
//	DrawRectangle(x,y/*-Ascent()*/,s.size()*8,16);
	_data->p->character_string(s.c_str(), 
														 Coord(x+_dx),
														 Coord(_data->h-(y+_dy+Ascent())));
}

void TPen::DrawBitmap(int x,int y,TBitmap *bmp)
{
	if (bmp && bmp->_data) {
#if 0
		_data->p->stencil(bmp->_data->bmp,
										Coord(x+_dx),
										Coord(_data->h-(y+_dy+bmp->height)));
#endif
		SetColor(TColor::BLACK);
		FillRectangle(x,y,bmp->width,bmp->height);
		_data->p->image(bmp->_data->bmp,
										Coord(x+_dx)*0.96,
										Coord(_data->h-(y+_dy+bmp->height))*0.96);
	}
}

void TPen::DrawLine(int x1,int y1,int x2,int y2)
{
	_data->p->begin_path();
	_data->p->move_to(Coord(x1+_dx),Coord(_data->h-(y1+_dy)));
	_data->p->line_to(Coord(x2+_dx),Coord(_data->h-(y2+_dy)));
	_data->p->close_path();
	_data->p->stroke();
}

void TPen::DrawRectangle(int x,int y,int w,int h)
{
	_data->p->rect(Coord(x+_dx),Coord(_data->h-(y+_dy)),
								 Coord(x+_dx+w),Coord(_data->h-(y+_dy+h)) );
	_data->p->stroke();
}

void TPen::FillRectangle(int x,int y,int w,int h)
{
	_data->p->rect(Coord(x+_dx),Coord(_data->h-(y+_dy)),
								 Coord(x+_dx+w),Coord(_data->h-(y+_dy+h)) );
	_data->p->fill();
}

void TPen::FillCircle(int ix1,int iy1,int ix2,int iy2)
{
	// Ahh, Fresco:

	static const float p0 = 1.00000000f;
	static const float p1 = 0.89657547f;   // cos 30 * sqrt(1 + tan 15 * tan 15)
	static const float p2 = 0.70710678f;   // cos 45
	static const float p3 = 0.51763809f;   // cos 60 * sqrt(1 + tan 15 * tan 15)
	static const float p4 = 0.26794919f;   // tan 15
	
	float x1=(float)ix1, y1=(float)iy1, x2=(float)ix2, y2=(float)iy2;

	float r1 = (x2-x1)/4.0f;
	float r2 = (y2-y1)/4.0f;
	
	float x = x1+r1+_dx;
	float y = (float)_data->h - (y1+r2+_dy);
	
	float px0 = p0 * r1, py0 = p0 * r2;
	float px1 = p1 * r1, py1 = p1 * r2;
	float px2 = p2 * r1, py2 = p2 * r2;
	float px3 = p3 * r1, py3 = p3 * r2;
	float px4 = p4 * r1, py4 = p4 * r2;

	Painter_ptr p = _data->p;
	p->begin_path();
	p->move_to(x + r1, y);
	p->curve_to(x + px2, y + py2, x + px0, y + py4, x + px1, y + py3);
	p->curve_to(x, y + r2, x + px3, y + py1, x + px4, y + py0);
	p->curve_to(x - px2, y + py2, x - px4, y + py0, x - px3, y + py1);
	p->curve_to(x - r1, y, x - px1, y + py3, x - px0, y + py4);
	p->curve_to(x - px2, y - py2, x - px0, y - py4, x - px1, y - py3);
	p->curve_to(x, y - r2, x - px3, y - py1, x - px4, y - py0);
	p->curve_to(x + px2, y - py2, x + px4, y - py0, x + px3, y - py1);
	p->curve_to(x + r1, y, x + px1, y - py3, x + px0, y - py4);
	p->close_path();
	p->fill();
}

void TPen::Translate(int dx,int dy)
{
	_dx+=dx;
	_dy+=dy;
}

void TPen::SetFont(TFont *font)
{
/*
	if (_font)
		delete _font;
*/
	_font = font;
	_data->p->current_font(_font->_data->f);
}

void TPen::SetColor(const TRGB &c)
{
	Color_var r = drawingkit->color_rgb(1.0/255.0*c.r, 1.0/255.0*c.g, 1.0/255.0*c.b);
	_data->p->current_color(r);
}

void TPen::SetColor(TColor::EColor e)
{
	switch(e) {
		case TColor::BLACK: {
				Color_var r = drawingkit->color_rgb(0.0, 0.0, 0.0);
				_data->p->current_color(r);
			}
			break;
		case TColor::LIGHTGRAY: {
				Color_var r = drawingkit->color_rgb(0.75, 0.75, 0.75);
				_data->p->current_color(r);
			}
			break;
	}
}

// TWindow
//---------------------------------------------------------------------------
class TWindow::TFrescoPainter: ixx_extends(GlyphImpl)
{
	TWindow *_master;
	public:
		TFrescoPainter(TWindow* master) {
			_master = master;
		}
		void request(Requisition& r, _Ix_Env& _env = _Ix_default_env);
		void draw(XfGlyphTraversal_ptr, _Ix_Env& = _Ix_default_env);
};

class TWindow::TFrescoSlave: ixx_extends(ViewerImpl)
{
		TWindow *_master;
	public:
		TFrescoSlave(TWindow *master);
		
		bool press(GlyphTraversal_ptr t, Event_ptr e);
		bool release(GlyphTraversal_ptr t, Event_ptr e);
		bool move(GlyphTraversal_ptr, Event_ptr);
		bool drag(GlyphTraversal_ptr t, Event_ptr e);
};

TWindow::TFrescoSlave::TFrescoSlave(TWindow *master)
{
	_master = master;
	body( new TWindow::TFrescoPainter(master) );
#if 0	
		layoutkit->back(
			new TWindow::TFrescoPainter(master),
			widgetkit->filler()
		)
	);
#endif
}

bool TWindow::TFrescoSlave::move(GlyphTraversal_ptr t, Event_ptr e)
{
	Vertex lower, upper, origin;
	t->bounds(lower, upper, origin);
	Vertex v;
	v.x = e->pointer_x();
	v.y = e->pointer_y();
	v.z = Coord(0);
	t->current_transform()->inverse_transform_vertex(v);
	_master->mouseMove(rint(v.x), rint(upper.y-v.y), 0);
}

bool TWindow::TFrescoSlave::drag(GlyphTraversal_ptr t, Event_ptr e)
{
	move(t,e);
}

bool TWindow::TFrescoSlave::press(GlyphTraversal_ptr t, Event_ptr e)
{
	Vertex lower, upper, origin;
	t->bounds(lower, upper, origin);
	Vertex v;
	v.x = e->pointer_x();
	v.y = e->pointer_y();
	v.z = Coord(0);
	t->current_transform()->inverse_transform_vertex(v);
	switch(e->pointer_button()) {
		case 1:
			_master->mouseLDown(rint(v.x)-_master->_dx, rint(upper.y-v.y)-_master->_dy, 0);
			break;
		case 2:
			_master->mouseMDown(rint(v.x), rint(upper.y-v.y), 0);
			break;
		case 3:
			_master->mouseRDown(rint(v.x), rint(upper.y-v.y), 0);
			break;
	}
}

bool TWindow::TFrescoSlave::release(GlyphTraversal_ptr t, Event_ptr e)
{
	Vertex lower, upper, origin;
	t->bounds(lower, upper, origin);
	Vertex v;
	v.x = e->pointer_x();
	v.y = e->pointer_y();
	v.z = Coord(0);
	t->current_transform()->inverse_transform_vertex(v);
	switch(e->pointer_button()) {
		case 1:
			_master->mouseLUp(rint(v.x), rint(upper.y-v.y), 0);
			break;
		case 2:
			_master->mouseMUp(rint(v.x), rint(upper.y-v.y), 0);
			break;
		case 3:
			_master->mouseRUp(rint(v.x), rint(upper.y-v.y), 0);
			break;
	}
}

void TWindow::TFrescoPainter::request(Glyph::Requisition& r, _Ix_Env&)
{
	r.x.defined = true;
	r.x.natural = _master->_w;
	r.x.maximum = 4096;
	r.x.minimum = 0;
	r.x.align = Alignment(0);
	r.y.defined = true;
	r.y.natural = _master->_h;
	r.y.maximum = 4096;
	r.y.minimum = 0;
	r.y.align = Alignment(0);
	r.preserve_aspect = false;
}

void TWindow::TFrescoPainter::draw(XfGlyphTraversal_ptr t, _Ix_Env&)
{
	_master->_traversal = t;
	Painter_var p = t->current_painter();
	Vertex lower, upper, origin;
	t->bounds(lower, upper, origin);
	p->push_clipping();
	p->clip_rect(lower.x, lower.y, upper.x, upper.y);

	int w = rint(upper.x-lower.x);
	int h = rint(upper.y-lower.y);
	if (w!=_master->_w || h!=_master->_h) {
cout<< "resizing window from "
		<< _master->_w << ", " << _master->_h << " to "
		<< upper.x-lower.x << ", " << upper.y-lower.y << " ("
		<< w << ", " << h << endl;
		_master->_w = w;
		_master->_h = h;
		_master->_slave->need_resize();
		_master->resize();
	}

	Color_var c = drawingkit->color_rgb(
		1.0/255.0*(float)_master->background.r,
		1.0/255.0*(float)_master->background.g,
		1.0/255.0*(float)_master->background.b
	);
	p->current_color(c);
	p->fill_rect(lower.x, lower.y, upper.x, upper.y);

	_master->paint();

	p->pop_clipping();
}

TWindow::TWindow(TWindow*, const string&)
{
	_slave = new TFrescoSlave(this);
	_w = 400;
	_h = 400;
	_dx = _dy = 0;
}

TWindow::~TWindow()
{
}

XfViewer* TWindow::GetViewer()
{
	return _slave;
}

void TWindow::SetSize(int,int)
{
}

void TWindow::SetShape(int,int,int,int)
{
}

void TWindow::SetBackground(const TRGB &rgb)
{
	background = rgb;
#if 0
	cout<< "Set Background to "
			<<  (int)background.r << ", " <<  (int)background.g << ", " <<(int)background.b
			<< endl;
#endif
}

void TWindow::SetBackground(TColor::EColor)
{
}

void TWindow::SetBackground(byte r,byte g,byte b)
{
	background.Set(r,g,b);
}

void TWindow::SetColor(TColor::EColor)
{
}

void TWindow::SetOrigin(int dx, int dy)
{
	_dx = dx; _dy = dy;
}

void TWindow::ScrollTo(int dx, int dy)
{
	_dx = dx; _dy = dy;
	Invalidate();
}

void TWindow::SetVisible(bool)
{
}

int TWindow::Width()
{
	return _w;
}

int TWindow::Height()
{
	return _h;
}

void TWindow::Invalidate()
{
	_slave->need_redraw();
}

void TWindow::mouseLDown(int,int,unsigned){}
void TWindow::mouseMDown(int,int,unsigned){}
void TWindow::mouseRDown(int,int,unsigned){}
void TWindow::mouseLUp(int,int,unsigned){}
void TWindow::mouseMUp(int,int,unsigned){}
void TWindow::mouseRUp(int,int,unsigned){}
void TWindow::mouseMove(int,int,unsigned){}
void TWindow::paint(){}
void TWindow::resize(){}

// TVScrollBar
//---------------------------------------------------------------------------
TVScrollBar::TVScrollBar(TWindow *p, const string &t)
	:TWindow(p,t)
{
	_w = 0;
}

int TVScrollBar::Value()
{
	return 0;
}

void TVScrollBar::SetValue(int)
{
}

void TVScrollBar::SetRange(int min,int max)
{
	this->min = min;
	this->max = max;
//	cout << "SetRange " << min << " - " << max << endl;
}
