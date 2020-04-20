#include "stdafx.h"
#include "figure.h"
#include <thread>
#include <vector>

typedef char IND;
struct PT {IND x, y;};

static const int pix = 15;
static const bool mirror = 1;
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

static const Color colors[] = {
	{0x80, 0x00, 0x00}, // maroon
	{0x9a, 0x63, 0x24}, // brown
	{0x80, 0x80, 0x00}, // olive
	{0x46, 0x99, 0x90}, // teal
	{0x00, 0x00, 0x75}, // navy
	{0xe6, 0x19, 0x48}, // red
	{0xf5, 0x82, 0x31}, // orange
	{0xff, 0xe1, 0x19}, // yellow
	{0xbf, 0xef, 0x45}, // lime
	{0x3c, 0xb4, 0x4b}, // green
	{0x42, 0xd4, 0xf4}, // cyan
	{0x43, 0x63, 0xd8}, // blue
	{0x91, 0x1e, 0xb4}, // purple
	{0xf0, 0x32, 0xe6}, // magenta
	{0xa9, 0xa9, 0xa9}, // grey
	{0xfa, 0xbe, 0xbe}, // pink
	{0xff, 0xd8, 0xb1}, // apricot
	{0xff, 0xfa, 0xc8}, // beige
	{0xaa, 0xff, 0xc3}, // mint
	{0xe6, 0xbe, 0xff}, // lavender
};

class Sizes {
	IND _w[figure_count];
	IND _h[figure_count];

public:
	static Sizes const& Get() {
		static Sizes sizes;
		return sizes;
	}

	IND Width(IND k) const {return _w[k];}
	IND Height(IND k) const {return _h[k];}

protected:
	Sizes() {
		for (IND k=0; k<figure_count; k++) {
			IND w = 0, h = 0;
			for (IND n=0; n<piece_count; n++) {
				auto pt = figures[k][n];
				if (w < pt.x)
					w = pt.x;
				if (h < pt.y)
					h = pt.y;
			}
			_w[k] = w + 1;
			_h[k] = h + 1;
		}
	}
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

	PT FirstHole() const {
		for (IND y=0; y<_h; y++) {
			for (IND x=0; x<_w; x++) {
				if (IsEmpty(x, y)) {
					return {x, y};
				}
			}
		}
		return {-1, -1};
	}

private:
	IND _data[_h][_w];
};

class HoleFinder {
public:
	HoleFinder(Field const& f) : _field(f), _hole{} {}

	bool Find(IND x, IND y) {
		if (_field.IsOutside(x, y))
			return false;
		if (!_visit.IsEmpty(x, y))
			return false;
		if (_field.IsEmpty(x, y))
		{
			_hole = {x, y};
			return true;
		}
		_visit.Set(x, y, 0);
		return Find(x, y-1) || Find(x-1, y) || Find(x, y+1) || Find(x+1, y);
	}

	PT Get() const {return _hole;}

private:
	Field _visit;
	Field const& _field;
	PT _hole;
};

class Figure {
public:
	Figure(IND k) : _color(k) {
		memcpy(_data, figures[k], sizeof(_data));
		_w = Sizes::Get().Width(k);
		_h = Sizes::Get().Height(k);
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

	bool FitIn(Field const& f, PT pt, IND index) const {
		IND dx = pt.x - _data[index].x;
		IND dy = pt.y - _data[index].y;
		for (IND n=0; n<piece_count; n++) {
			IND x = _data[n].x + dx;
			IND y = _data[n].y + dy;
			if (f.IsOutside(x, y))
				return false;
			if (!f.IsEmpty(x, y)) {
				return false;
			}
		}
		return true;
	}

	void MarkIn(Field& f, PT pt, IND index) const {
		IND dx = pt.x - _data[index].x;
		IND dy = pt.y - _data[index].y;
		for (IND n=0; n<piece_count; n++) {
			IND x = _data[n].x + dx;
			IND y = _data[n].y + dy;
			f.Set(x, y, _color);
		}
	}

private:
	IND _w, _h;
	IND _color;
	PT _data[piece_count];
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

typedef std::vector<Field> Solution;
Solution sol;

struct Context {
	Solution& sol;
	int dbgcnt;
};

struct Frame {
	Field field;
	Index index;
	PT hole;

	Frame()
		: hole{field.FirstHole()} {}

	Frame(const Frame* p, IND skip)
		:	field(p->field), index(p->index, skip), hole{} {}

	Frame(const Field& f, const Index& i, IND skip)
		:	field(f), index(i, skip), hole{} {}

	void Solve(Context& cntx) {
		for (IND k=0; k<index.Count(); k++)
			Solve1(cntx, k);
	}

	void Solve1(Context& cntx, IND k) {
		cntx.dbgcnt++;
		IND x = index.Get(k);
		Figure fig(x);
		IND nm = mirror ? mirs[x] : 1;
		for (char mir=0; mir<nm; mir++) {
			for (char rot=0; rot<rots[x]; rot++) {
				for (IND n=0; n<piece_count; n++) {
					if (fig.FitIn(field, hole, n)) {
						Frame fr(this, k);
						fig.MarkIn(fr.field, hole, n);
						if (fr.IsDone(cntx))
							break;
						HoleFinder hf(fr.field);
						if (hf.Find(hole.x, hole.y)) {
							fr.hole = hf.Get();
							fr.Solve(cntx);
						}
					}
				}
				fig.Rotate();
			}
			fig.Mirror();
		}
	}

protected:
	bool IsDone(Context& cntx) {
		if (index.Count() )
			return false;
		cntx.sol.push_back(field);
		return true;
	}
};

struct TC {
	Solution sol;
	Context cntx;
	TC() : cntx{sol} {}
};

TC tcs[figure_count];

static void solve1(IND k) {
	Context& cntx = tcs[k].cntx;
	Frame fr0;
	fr0.Solve1(cntx, k);
}

static void solveMT() {
	Elapsed el;
	Context cntx{sol};
	{
		std::thread ths[figure_count];
		for (IND k=0; k<figure_count; k++)
			ths[k] = std::thread(solve1, k);
		for (auto& th : ths)
			th.join();
	}
	for (auto& tc : tcs) {
		cntx.dbgcnt += tc.cntx.dbgcnt;
		cntx.sol.insert(cntx.sol.end(), tc.cntx.sol.begin(), tc.cntx.sol.end());
	}
	auto sec = el.sec();
	
	printf("solveMT():\n");
	printf("elapsed = %g sec\n", sec);
	printf("dbgcnt = %d\n", cntx.dbgcnt);
	printf("solutions = %d\n", (int)sol.size());
}

static void solve() {
	Elapsed el;
	Context cntx{sol};
	{
		Frame fr;
		fr.Solve(cntx);
	}
	auto sec = el.sec();
	printf("solve():\n");
	printf("elapsed = %g sec\n", sec);
	printf("dbgcnt = %d\n", cntx.dbgcnt);
	printf("solutions = %d\n", (int)sol.size());
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
	solveMT();
	sol.resize(0);
	solve();
	_brushes = new Brush[figure_count+1];
	for (int i=0; i<figure_count; i++)
		_brushes[i].Create(colors[i]);
	_brushes[figure_count].Create(Color(0xff, 0xff, 0xff));
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
