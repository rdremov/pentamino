#pragma once

#include "gdi.h"
#include "elapsed.h"
#include <vector>

class MainWnd : public Wnd {
public:
	MainWnd() {}
	~MainWnd() {}
	bool OnPaint(DC* pDC) override;
};
