#pragma once

#include "../ui/gdi.h"
#include "../ui/elapsed.h"

class MainWnd : public Wnd {
public:
	MainWnd();
	~MainWnd();
	bool OnPaint(DC* pDC) override;
private:
	Brush* _brushes;
};
