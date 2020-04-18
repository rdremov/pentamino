#pragma once
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

typedef wchar_t CH;

class Instance;
class DC;

class Key {
	int _vk;
public:
	Key(WPARAM vk) : _vk((int)vk) {}
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
	
	void Create(Instance& inst, const CH* title);
	void Destroy() {DestroyWindow(_h);}
	void Show(int nShow) {ShowWindow(_h, nShow);}
	void Update() {UpdateWindow(_h);}
	void Invalidate(bool eraseBk = true) {InvalidateRect(_h, nullptr, eraseBk);}

	virtual bool OnPaint(DC* pDC) {return true;}
	virtual bool OnKeyDown(Key key) {return true;}
	virtual bool OnKeyUp(Key key) {return true;}
};

class Instance : public Handle<HINSTANCE> {
	HACCEL _hAccel;
	Wnd* _pWnd;
public:
	Instance(HINSTANCE hInst, Wnd* pWnd) {
		_h = hInst;
		_hAccel = nullptr;
		_pWnd = pWnd;
	}
	~Instance() {_h = nullptr;}
	void Init(UINT idRes, int nCmdShow);
	int Run();
};
