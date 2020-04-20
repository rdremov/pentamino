#pragma once
#include "wnd.h"

class Color {
public:
	union {
		unsigned int _rgb;
		struct {
			int _r:8;
			int _g:8;
			int _b:8;
		};
	};
	Color(int r, int g, int b) : _r(r), _g(g), _b(b) {}
};

class DC : public Handle<HDC> {
public:
	void Rect(int l, int t, int r, int b) {Rectangle(_h, l, t, r, b);}
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

class Pen : public Handle<HPEN> {
public:
	Pen(int width, int rgb = 0) {
		_h = CreatePen(PS_SOLID, width, rgb);
	}
	~Pen() {
		if (_h) {
			DeleteObject(_h);
			_h = nullptr;
		}
	}
};

class Brush : public Handle<HBRUSH> {
public:
	Brush() {}
	~Brush() {
		if (_h) {
			DeleteObject(_h);
			_h = nullptr;
		}
	}
	void Create(Color color) {
		_h = CreateSolidBrush(color._rgb);
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
