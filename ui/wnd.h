#pragma once

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <assert.h>

template<class T>
class Handle {
public:
	T _h;
protected:
	Handle(T h = 0) : _h(h) {}
	virtual ~Handle() {assert(_h == 0);}
};

class Instance;
class DC;

class Key {
	int _vk;
public:
	Key(WPARAM vk) : _vk((int)vk) {}
	operator int() const {return _vk;}
};

class Wnd : public Handle<HWND> {
protected:
	bool _default;
public:
	Wnd() : _default(false) {}
	~Wnd() {
		if (_h) {
			Destroy();
			_h = nullptr;
		}
	}
	
	void Create(Instance& inst, const char* title);
	void Destroy() {DestroyWindow(_h);}
	void Show(int nShow) {ShowWindow(_h, nShow);}
	void Update() {UpdateWindow(_h);}
	void Invalidate(bool eraseBk = true) {InvalidateRect(_h, nullptr, eraseBk);}
	void Invalidate(int l, int t, int r, int b, bool eraseBk) {
		RECT rect = {l, t, r, b};
		InvalidateRect(_h, &rect, eraseBk);
	}

	void GetSize(int& w, int& h) const {
		RECT r{};
		GetClientRect(_h, &r);
		w = r.right - r.left;
		h = r.bottom - r.top;
	}

	static LRESULT CALLBACK Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	virtual bool OnPaint(DC* pDC) {return false;}
	virtual bool OnKeyDown(Key key) {return false;}
	virtual bool OnKeyUp(Key key) {return false;}
	virtual bool OnLButtonDown(int x, int y) {return false;}
	virtual bool OnLButtonUp(int x, int y) {return false;}
	virtual bool OnMouseMove(int x, int y) {return false;}
};

class Instance : public Handle<HINSTANCE> {
	Wnd* _pWnd;
public:
	Instance(HINSTANCE hInst, Wnd* pWnd) {
		_h = hInst;
		_pWnd = pWnd;
	}
	~Instance() {_h = nullptr;}
	void Init(int nCmdShow);
	int Run();
};

struct FileName {
	char name[1024];
	FileName(bool save);
};
