#pragma once
#include "wnd.h"

class Color {
	union {
		unsigned int _rgb;
		struct {
			int _r:8;
			int _g:8;
			int _b:8;
		};
	};
public:
	Color() : _rgb(0) {}
	Color(int r, int g, int b) : _r(r), _g(g), _b(b) {}
	operator int() {return _rgb;}

	static const Color black;
	static const Color white;
	static const Color red;
	static const Color green;
	static const Color blue;
	static const Color grey;
	static const Color ltgrey;
	static const Color dkgrey;
};

class DC : public Handle<HDC> {
public:
	void MoveTo(int x, int y) {MoveToEx(_h, x, y, NULL);}
	void LineTo(int x, int y) {::LineTo(_h, x, y);}
	void Line(int x1, int y1, int x2, int y2) {MoveTo(x1, y1); LineTo(x2, y2);}
	void Rect(int l, int t, int r, int b) {Rectangle(_h, l, t, r, b);}
	void Text(int x, int y, const char* text) {TextOut(_h, x, y, text, lstrlen(text));}
};

class PaintDC : public DC {
	PAINTSTRUCT _ps;
	Wnd* _pWnd;

public:
	PaintDC(Wnd* pWnd) : _pWnd(pWnd) {
		_h = BeginPaint(_pWnd->_h, &_ps);
	}
	~PaintDC() {
		_h = nullptr;
		EndPaint(_pWnd->_h, &_ps);
	}
};

template<class T>
class GdiHandle : public Handle<T> {
public:
	~GdiHandle() {
		T& h = this->_h;
		if (h) {
			DeleteObject(h);
			h = nullptr;
		}
	}
};

class Pen : public GdiHandle<HPEN> {
public:
	Pen() {
		_h = CreatePen(PS_NULL, 0, 0);
	}
	Pen(int width) {
		_h = CreatePen(PS_SOLID, width, 0);
	}
	Pen(int width, Color color) {
		_h = CreatePen(PS_SOLID, width, color);
	}
};

class Brush : public GdiHandle<HBRUSH> {
public:
	Brush() {}
	Brush(Color color) {Create(color);}
	void Create(Color color) {
		_h = CreateSolidBrush(color);
	}
};

class Font : public GdiHandle<HFONT> {
public:
	Font() {}
	Font(int height) {
		LOGFONT lf{height};
		_h = CreateFontIndirect(&lf);
	}
};

class Select {
	HGDIOBJ _hOld;
	DC* _pDC;
public:
	template<class T>
	Select(DC* pDC, T const& obj) : _pDC(pDC) {
		_hOld = SelectObject(_pDC->_h, obj._h);
	}
	~Select() {
		SelectObject(_pDC->_h, _hOld);
	}
};
