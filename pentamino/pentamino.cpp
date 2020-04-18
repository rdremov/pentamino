// pentamino.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "pentamino.h"
#include "figure.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MainWnd wnd;
	Instance inst(hInstance, &wnd);
	inst.Init(IDC_PENTAMINO, nCmdShow);
	return inst.Run();
}
