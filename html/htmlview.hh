#include <toad/toad.hh>

#include <vector>

class TScrollBar;
class TVScrollBar;

class TInit;
class TElementVector;
class TAnchor;
class TAnchorVector;
class TLineVector;

class THistory
{
		string *buffer;
		unsigned size;
		unsigned pos;
		unsigned start, end;
	public:
		THistory(unsigned size);
		~THistory();
		void Go(const string &);
		void Next();
		void Prev();
		const string& Current() const {
			if (pos==size) {
				cout << "history is empty" << endl;
				exit(1);
			}
//			cout << "current is " << buffer[pos] << endl;
			return buffer[pos];
		}
		void Print();
};

class THTMLView
	:public TWindow
{
		string _filename;

	public:		
		TVScrollBar *_vscroll;
//		void actVScroll(TIntManipulator*);
		void actVScroll(TScrollBar*);
		THTMLView(TWindow*, const string&);
		~THTMLView();
		void SetValue(const string& html);
		void Open(const string& url);
		void _Open(const string& file);
		
		void Init();
		static void Init(TInit &init, int stage);
		void mouseLDown(int,int,unsigned);
		void mouseMove(int,int,unsigned);
		void paint();
		void resize();
		static void paint(TPen &pen, TInit &init);
		TElementVector *parsed;
		TLineVector *lines;
		TAnchorVector *anchors;
		TAnchor *_current;
		
		// history
		//---------
		THistory history;
		
		void Back();
		void Forward();
};
