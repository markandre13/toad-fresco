class XfPainter;

class TDPoint
{
	public:
		double x, y;
};

class TCTool
{
	public:
		virtual void mouseLDown(TDPoint&);
		virtual void mouseMDown(TDPoint&);
		virtual void mouseRDown(TDPoint&);
		virtual void mouseLUp(TDPoint&);
		virtual void mouseMUp(TDPoint&);
		virtual void mouseRUp(TDPoint&);
		virtual void mouseMove(TDPoint&);
		
		virtual void paint(XfPainter*);
};

class TCToolRect: public TCTool
{
	public:
		TCToolRect();
		void mouseLDown(TDPoint&);
		void mouseLUp(TDPoint&);
		void mouseMove(TDPoint&);
		void paint(XfPainter*);
	private:
		int _state;
		TDPoint _start, _end;
};
