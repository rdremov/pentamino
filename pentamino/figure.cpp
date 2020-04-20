#include "stdafx.h"
#include "figure.h"

typedef char IND;
struct PT {IND x, y;};

static const int pix = 15;
static const IND piece_count = 5;
static const IND figure_count = 12;

static const PT figures[figure_count][piece_count] = {
	0,0, 2,0, 0,1, 1,1, 2,1, // c
	1,0, 0,1, 1,1, 2,1, 1,2, // +
	1,0, 0,1, 1,1, 0,2, 1,2, // p
	1,0, 0,1, 1,1, 1,2, 2,2, // t
	0,0, 0,1, 0,2, 0,3, 0,4, // I
	0,0, 1,0, 2,0, 1,1, 1,2, // T
	1,0, 2,0, 3,0, 0,1, 1,1, // 4
	1,0, 2,0, 0,1, 1,1, 0,2, // W
	0,0, 1,0, 1,1, 1,2, 2,2, // Z
	2,0, 2,1, 0,2, 1,2, 2,2, // L
	0,0, 0,1, 1,1, 2,1, 3,1, // J
	2,0, 0,1, 1,1, 2,1, 3,1, // h
};

static const char rots[figure_count] = {
	4, 1, 4, 4, 2, 4, 4, 4, 2, 4, 4, 4
};

static const char mirs[figure_count] = {
	1, 1, 2, 2, 1, 1, 2, 1, 2, 1, 2, 2
};

static const Color colors[figure_count+1] = {
	{0xcc, 0xff, 0xcc},
	{0xff, 0xcc, 0x33},
	{0xcc, 0x99, 0xff},
	{0xcc, 0x66, 0x99},
	{0xff, 0x66, 0x33},
	{0x99, 0x66, 0xcc},
	{0x33, 0x66, 0x99},
	{0x00, 0xcc, 0xff},
	{0xcc, 0xff, 0xff},
	{0x33, 0xff, 0x33},
	{0xff, 0x33, 0x33},
	{0x66, 0x66, 0x66},
	{0xd0, 0xd0, 0xd0}, // vacant
};

class Field {
	static const IND _w = 10;
	static const IND _h = 6;

public:
	Field(IND v=figure_count) {memset(_data, v, sizeof(Field));}
	Field(const Field& f) {memcpy(_data, f._data, sizeof(Field));}
	bool IsOutside(IND x, IND y) const {return x < 0 || x >= _w || y < 0 || y >= _h;}
	bool IsEmpty(IND x, IND y) const {return _data[y][x] == figure_count;}
	void Set(IND x, IND y, IND v) {_data[y][x] = v;}
	IND	Get(IND x, IND y) const {return _data[y][x];}
	IND Width() const {return _w;}
	IND Height() const {return _h;}

private:
	IND _data[_h][_w];
};

class Piece {
public:
	Piece(IND k) : _w(0), _h(0), _color(k) {
		memcpy(_data, figures[k], sizeof(_data));
		for (IND n=0; n<piece_count; n++) {
			auto pt = _data[n];
			if (_w < pt.x)
				_w = pt.x;
			if (_h < pt.y)
				_h = pt.y;
		}
		_w++, _h++;
	}

	void Rotate() {
		IND temp = _w;
		_w = _h;
		_h = temp;

		for (IND n=0; n<piece_count; n++) {
			auto& pt = _data[n];
			temp = pt.x, pt.x = -pt.y + _w, pt.y = temp;
		}
	}

	void Mirror() {
		for (IND n=0; n<piece_count; n++)
			_data[n].x = -_data[n].x + _w - 1;
	}

	bool Fit(const Field& f, PT pt, IND index) const {
		IND dx = pt.x - _data[index].x;
		IND dy = pt.y - _data[index].y;
		for (IND n=0; n<piece_count; n++) {
			IND x = _data[n].x + dx, y = _data[n].y + dy;
			if (f.IsOutside(x, y))
				return false;
			if (!f.IsEmpty(x, y)) {
				return false;
			}
		}
		return true;
	}

	void Mark(Field& f, PT pt, IND index) const {
		IND dx = pt.x - _data[index].x;
		IND dy = pt.y - _data[index].y;
		for (IND n=0; n<piece_count; n++)
			f.Set(_data[n].x + dx, _data[n].y + dy, _color);
	}

private:
	IND _w, _h;
	IND _color;
	PT _data[piece_count];
};

typedef std::vector<Field> Solution;

struct Context {
	Solution& sol;
	int dbgcount;
};

class Index {
	IND _count;
	IND _data[figure_count];

public:
	Index() : _count(figure_count) {
		for (IND k=0; k<_count; k++)
			_data[k] = k;
	}

	Index(const Index& index, IND skip) : _count(index._count - 1) {
		for (IND k=0; k<skip; k++)
			_data[k] = index._data[k];
		for (IND k=skip; k<_count; k++)
			_data[k] = index._data[k+1];
	}

	IND Count() const {return _count;}
	IND Get(IND k) const {return _data[k];}
};

class Figure {
	Field _field;
	PT _hole;
	Index _index;

public:
	Figure() : _hole{} {}
	Figure(const Figure* p, IND skip)
		:	_field(p->_field),
			_hole{},
			_index(p->_index, skip) {
	}

	bool Solve(Context& cntx) {
		cntx.dbgcount++;

		for (IND k=0; k<_index.Count(); k++) {
			IND x = _index.Get(k);
			Piece piece(x);
			IND nm = 1;//mirs[x];
			for (char mir=0; mir<nm; mir++) {
				for (char rot=0; rot<rots[x]; rot++) {
					for (IND n=0; n<piece_count; n++) {
						if (piece.Fit(_field, _hole, n)) {
							Figure fig(this, k);
							piece.Mark(fig._field, _hole, n);
							if (fig.IsDone(cntx))
								break;//return true;
							Field visit;
							if (fig.Hole(visit, _hole.x, _hole.y)) {
								if (fig.Solve(cntx))
									return true;
							 }
						}
					}
					piece.Rotate();
				}
				piece.Mirror();
			}
		}

		return false;
	}

protected:
	bool IsDone(Context& cntx) {
		if (_index.Count() )
			return false;
		cntx.sol.push_back(_field);
		return true;
	}

	bool Hole(Field& visit, IND x, IND y) {
		if (_field.IsOutside(x, y))
			return false;
		if (!visit.IsEmpty(x, y))
			return false;
		if (_field.IsEmpty(x, y))
		{
			_hole = {x, y};
			return true;
		}
		visit.Set(x, y, 0);
		return
			Hole(visit, x, y-1) ||
			Hole(visit, x-1, y) ||
			Hole(visit, x, y+1) ||
			Hole(visit, x+1, y);
	}
};

Solution sol;

static void solve() {	
	Elapsed el;
	Context cntx{sol};
	Figure fig;
	fig.Solve(cntx);
	auto sec = el.sec();
}

static void draw_field(Field const& f, DC* pDC, Brush* brushes, int x0, int y0) {
	int y = y0;
	for (IND j=0; j<f.Height(); j++) {
		int x = x0;
		for (IND i=0; i<f.Width(); i++) {
			Select sb(pDC, brushes[f.Get(i, j)]);
			pDC->Rect(x, y, x + pix + 1, y + pix + 1);
			x += pix;
		}
		y += pix;
	}
}

MainWnd::MainWnd() {
	solve();
	_brushes = new Brush[figure_count+1];
	for (int i=0; i<=figure_count; i++)
		_brushes[i].Create(colors[i]);
}

MainWnd::~MainWnd() {
	delete[] _brushes;
}

bool MainWnd::OnPaint(DC* pDC) {
	if (sol.empty())
		return false;

	int w, h;
	GetSize(w, h);

	int w1 = (1 + sol[0].Width()) * pix;
	int h1 = (1 + sol[0].Height()) * pix;
	int nx = w / w1;

	Pen pen(1);
	Select sp(pDC, pen);

	int n = 0, y = pix;
	bool done = false;
	for (int j=0; !done && y<h; j++) {
		int x = pix;
		for (int i=0; !done && i<nx; i++) {
			draw_field(sol[n], pDC, _brushes, x, y);
			x += w1;
			if (++n == sol.size())
				done = true;
		}
		y += h1;
	}

	return false;
}
