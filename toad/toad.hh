#ifndef _TOAD_WRAPPER
#define _TOAD_WRAPPER

#include <typeinfo>
#include <string>
#include <toad/types.hh>

class TWindow;

struct TRGB
{
		byte r,g,b;

		friend bool operator ==(class TRGB,class TRGB);
		bool operator !=(const TRGB &c) {
			return (r==c.r && g==c.g && b==c.b)?false:true;
		}
		void operator() (byte rn,byte gn,byte bn) {
			r=rn;g=gn;b=bn;
		}
		void Set(byte rn,byte gn,byte bn) {
			r=rn;g=gn;b=bn;
		}
};

class TColor: public TRGB
{
	public:
		enum EColor {
			BLACK,
			LIGHTGRAY
		};
};

class TPen;

class TFont
{
		friend TPen;
		class TData;
		TData *_data;
	public:
		enum EFamily {
			SANS=0, SANSSERIF=0,
			SERIF=1,
			TYPEWRITER=3
		};
		
		enum EStyle {
			PLAIN = 0, REGULAR = 0,
			BOLD = 1, ITALIC = 2,
			BOLD_ITALIC = 3,
			OBLIQUE = 4,
			BOLD_OBLIQUE = 5,
			UNDERLINE = 6
		};
	
		TFont();
		TFont(EFamily,EStyle,int);
		~TFont();
		int TextWidth(const string&);
		int Ascent();
		int Descent();
};

class XfViewer;
class XfGlyphTraversal;

class TBitmap
{
	public:
		class TData;
		TData *_data;
		TBitmap();
		~TBitmap();
		int width, height;
		bool Load(const string&);
};

class TPen
{
		TFont *_font;
		class TData;
		TData *_data;
		TWindow *_wnd;
	public:
		TPen(TWindow*);
		~TPen();
		int TextWidth(const string &s) {return _font->TextWidth(s);}
		int Ascent() {return _font->Ascent();}
		int Descent() {return _font->Descent();}
		void DrawString(int,int,const string&);
		void DrawBitmap(int,int,TBitmap*);
		void DrawLine(int,int,int,int);
		void DrawRectangle(int,int,int,int);
		void FillRectangle(int,int,int,int);
		void FillCircle(int,int,int,int);
		int _dx, _dy;
		void Translate(int,int);
		void SetFont(TFont*);
		void SetColor(const TRGB&);
		void SetColor(TColor::EColor);
};

#define TMMM_ALL 0
#define CONNECT(a,b,c,d)


class TWindow
{
	public:
		TRGB background;
		class TFrescoSlave;
		class TFrescoPainter;
		
		TFrescoSlave *_slave;
		XfGlyphTraversal* _traversal;
		
		XfViewer* GetViewer();
		
		TWindow(TWindow*, const string&);
		virtual ~TWindow();
		void SetSize(int,int);
		void SetShape(int,int,int,int);
		void SetBackground(const TRGB&);
		void SetBackground(TColor::EColor);
		void SetBackground(byte,byte,byte);
		void SetColor(TColor::EColor);
		void SetMouseMoveMessages(int) {};
		void SetOrigin(int,int);
		void ScrollTo(int,int);
		void SetVisible(bool);
		int Width();
		int Height();
		void Invalidate();
		int _dx,_dy;
		
		int _w, _h;
		
		virtual void mouseLDown(int,int,unsigned);
		virtual void mouseMDown(int,int,unsigned);
		virtual void mouseRDown(int,int,unsigned);
		virtual void mouseLUp(int,int,unsigned);
		virtual void mouseMUp(int,int,unsigned);
		virtual void mouseRUp(int,int,unsigned);
		virtual void mouseMove(int,int,unsigned);
		virtual void paint();
		virtual void resize();
};

class TVScrollBar:
	public TWindow
{
	public:
		int min, max;
		TVScrollBar(TWindow*, const string&);
		int Value();
		void SetValue(int);
		void SetRange(int,int);
		int FixedSize() {return 16;}
};

#endif
