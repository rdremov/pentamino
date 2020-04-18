#include "stdafx.h"
#include "wnd.h"
#include "gdi.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Wnd* pWnd = (Wnd*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
	bool bDefault = false;

	switch (message)
	{
	case WM_PAINT:
		{
			PaintDC dc(pWnd);
			bDefault = pWnd->OnPaint(&dc);
		}
		break;
	case WM_KEYDOWN:
		bDefault = pWnd->OnKeyDown(wParam);
		break;
	case WM_KEYUP:
		bDefault = pWnd->OnKeyUp(wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		bDefault = true;
		break;
	}

	if (bDefault)
		return DefWindowProc(hWnd, message, wParam, lParam);
	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.hInstance      = hInstance;
	//wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PENTAMINO));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	//wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PENTAMINO);
	wcex.lpszClassName  = L"rvd";
	//wcex.hIconSm        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassExW(&wcex);
}

void Instance::Init(UINT idRes, int nCmdShow) {
	_hAccel = LoadAccelerators(_h, MAKEINTRESOURCE(idRes));

	#define MAX_LOADSTRING 256
	CH title[MAX_LOADSTRING];
	//LoadStringW(_h, IDS_APP_TITLE, title, MAX_LOADSTRING);
	wcscpy(title, L"junk");

	MyRegisterClass(_h);

	_pWnd->Create(*this, title);
	_pWnd->Show(nCmdShow);
	_pWnd->Update();
}

void Wnd::Create(Instance& inst, const CH* title) {
	_h = CreateWindow(L"rvd", title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, inst._h, nullptr);
	SetWindowLongPtr(_h, GWLP_USERDATA, (LONG_PTR)this);
}

int Instance::Run() {
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, _hAccel, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}
