// pentamino.cpp : Defines the entry point for the application.
//

#include "figure.h"
#include "../ui/console.h"

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpCmdLine, int nCmdShow) {
	Console cons;
	UNREFERENCED_PARAMETER(hPrevInst);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MainWnd wnd;
	Instance inst(hInst, &wnd);
	inst.Init(nCmdShow);
	return inst.Run();
}
