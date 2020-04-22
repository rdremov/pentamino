#include "gdi.h"

#define LPARAM_X(_l) ((short)_l)
#define LPARAM_Y(_l) ((short)((_l) >> 16))

LRESULT CALLBACK Wnd::Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Wnd* pWnd = (Wnd*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
	bool done = false;

	switch (message)
	{
	case WM_PAINT:
		{
			PaintDC dc(pWnd);
			done = pWnd->OnPaint(&dc);
		}
		break;
	case WM_KEYDOWN:
		done = pWnd->OnKeyDown(wParam);
		break;
	case WM_KEYUP:
		done = pWnd->OnKeyUp(wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
		done = pWnd->OnMouseMove(LPARAM_X(lParam), LPARAM_Y(lParam));
		break;
	case WM_LBUTTONDOWN:
		done = pWnd->OnLButtonDown(LPARAM_X(lParam), LPARAM_Y(lParam));
		break;
	case WM_LBUTTONUP:
		done = pWnd->OnLButtonUp(LPARAM_X(lParam), LPARAM_Y(lParam));
		break;
	default:
		break;
	}

	if (!done)
		return DefWindowProc(hWnd, message, wParam, lParam);
	return 0;
}

void MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = Wnd::Proc;
	wcex.hInstance      = hInstance;
	//wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PENTAMINO));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	//wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PENTAMINO);
	wcex.lpszClassName  = "rvd";
	//wcex.hIconSm        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
	auto ret = RegisterClassEx(&wcex);
}

void Instance::Init(const char* title, int nCmdShow) {
	MyRegisterClass(_h);

	_pWnd->Create(*this, title);
	_pWnd->Show(nCmdShow);
	_pWnd->Update();
}

void Wnd::Create(Instance& inst, const char* title) {
	_h = CreateWindow("rvd", title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, inst._h, nullptr);
	SetWindowLongPtr(_h, GWLP_USERDATA, (LONG_PTR)this);
}

int Instance::Run() {
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int) msg.wParam;
}

#include <commdlg.h>
FileName::FileName(bool save) {
	memset(name, 0, sizeof(name));
	char filter[] = "Pentamino file\0*.pnt\0";
	OPENFILENAME ofn{};
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = name;
	ofn.nMaxFile = sizeof(name);
	ofn.lpstrDefExt = "pnt";
	BOOL ret = FALSE;
	if (save)
		ret = GetSaveFileName(&ofn);
	else
		ret = GetOpenFileName(&ofn);
}
