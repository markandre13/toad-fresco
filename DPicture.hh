#include <list>

class TDShape;

class TDLayer
{
	public:
		void Add(TDShape*);
		void Remove(TDShape*);
		void Lower(TDShape*);
		void Raise(TDShape*);
		void Background(TDShape*);
		void Foreground(TDShape*);
	private:
		typedef list<TDShape*> TDShapeBuffer;
		TDShapeBuffer _shape;
};
