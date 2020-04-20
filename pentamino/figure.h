#pragma once

#include "gdi.h"
#include "elapsed.h"

class MainWnd : public Wnd {
public:
	MainWnd();
	~MainWnd();
	bool OnPaint(DC* pDC) override;
private:
	Brush* _brushes;
};
