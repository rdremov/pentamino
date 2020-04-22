#pragma once

#include <vector>
#include <stdio.h>

typedef char IND;
struct PT {IND x, y;};

static const IND piece_count = 5;
static const IND figure_count = 12;
static const IND total_count = figure_count * piece_count;

class Solution;

struct Context {
	Solution& sol;
	bool mirror;
	long long dbgcnt;
};

class Field {
public:
	static const IND N = 22;
	static const IND NIL = -1;

	// builder
	Field() : _w(N), _h(N) {
		memset(_data, NIL, sizeof(_data));
	}

	// hole finder
	Field(IND w, IND h) : _w(w), _h(h) {
		memset(_data, figure_count, sizeof(_data));
	}

	// solve
	Field(const Field& f) : _w(f._w), _h(f._h) {
		memcpy(_data, f._data, sizeof(_data));
	}

	// default
	void CreateRect() {
		_w = 10, _h = 6;
		for (IND j=0; j<_h; j++)
			for (IND i=0; i<_w; i++)
				_data[j][i] = figure_count;
	}

	void Load(const char* name) {
		FILE* pf = nullptr;
		fopen_s(&pf, name, "rb");
		if (pf) {
			_w = fgetc(pf);
			_h = fgetc(pf);
			for (IND j=0; j<_h; j++)
				for (IND i=0; i<_w; i++)
					_data[j][i] = fgetc(pf);
			fclose(pf);
		}
	}

	void Save(const char* name, IND l, IND t, IND r, IND b) {
		FILE* pf = nullptr;
		fopen_s(&pf, name, "wb");
		if (pf) {
			fputc(r - l + 1, pf);
			fputc(b - t + 1, pf);
			for (IND j=t; j<=b; j++)
				for (IND i=l; i<=r; i++)
					fputc(_data[j][i], pf);
			fclose(pf);
		}
	}

	bool IsOutside(IND x, IND y) const {
		return x < 0 || x >= _w || y < 0 || y >= _h || _data[y][x] == NIL;
	}
	
	bool IsEmpty(IND x, IND y) const {
		return _data[y][x] == figure_count;
	}

	void Toggle(IND x, IND y) {
		_data[y][x] = IsEmpty(x, y) ? NIL : figure_count;
	}

	void Toggle(IND x, IND y, IND v) {
		_data[y][x] = (v == figure_count) ? NIL : figure_count;
	}

	void Set(IND x, IND y, IND v) {_data[y][x] = v;}
	IND Get(IND x, IND y) const {return _data[y][x];}
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

	int Bounds(int& l, int& t, int& r, int& b) const {
		int cnt = 0;
		bool first = true;
		for (int j=0; j<N; j++) {
			for (int i=0; i<N; i++) {
				if (IsEmpty(i, j)) {
					if (first) {
						first = false;
						l = r = i;
						t = b = j;
					} else {
						if (l > i) l = i;
						if (t > j) t = j;
						if (r < i) r = i;
						if (b < j) b = j;
					}
					cnt++;
				}
			}
		}
		return cnt;
	}

	void Solve(Context& cntx);
	void SolveMT(Context& cntx);

private:
	IND _data[N][N];
	IND _w, _h;
};

class Solution : public std::vector<Field> {};

