#include "figure.h"
#include <thread>

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

class HoleFinder {
public:
	HoleFinder(Field const& f) : _field(f), _hole{}, _visit(f.Width(), f.Height()) {}

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
		if (_visit.Width() - x < _visit.Height() - y)
			return Find(x-1, y) || Find(x, y-1) || Find(x+1, y) || Find(x, y+1);
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

struct Frame {
	Field field;
	Index index;
	PT hole;

	Frame(const Field* p)
		: field(*p), hole{p->FirstHole()} {}

	Frame(const Frame* p, IND skip)
		: field(p->field), index(p->index, skip), hole{} {}

	Frame(const Field& f, const Index& i, IND skip)
		: field(f), index(i, skip), hole{} {}

	void Solve(Context& cntx) {
		for (IND k=0; k<index.Count(); k++)
			Solve1(cntx, k);
	}

	void Solve1(Context& cntx, IND k) {
		cntx.dbgcnt++;
		IND x = index.Get(k);
		Figure fig(x);
		IND nm = cntx.mirror ? mirs[x] : 1;
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

static void solve1(Context* pCntx, IND k, const Field* pField) {
	Frame fr(pField);
	fr.Solve1(*pCntx, k);
}

void Field::SolveMT(Context& cntx) {
	struct TC {
		std::thread th;
		Solution sol;
		Context cntx;
		TC() : cntx{sol} {}
		Context* Cntx(bool mirror) {cntx.mirror = mirror; return &cntx;}
	};
	TC tcs[figure_count];

	{
		for (IND k=0; k<figure_count; k++)
			tcs[k].th = std::thread(solve1, tcs[k].Cntx(cntx.mirror), k, this);
		for (auto& tc : tcs)
			tc.th.join();
	}
	for (auto& tc : tcs) {
		cntx.dbgcnt += tc.cntx.dbgcnt;
		cntx.sol.insert(cntx.sol.end(), tc.cntx.sol.begin(), tc.cntx.sol.end());
	}
}

void Field::Solve(Context& cntx) {
	Frame fr(this);
	fr.Solve(cntx);
}
