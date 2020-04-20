// pentamino.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "pentamino.h"
#include "figure.h"

#include <io.h>
#include <fcntl.h>
#include <iostream>

class Console {
public:
	Console(short minLength = 1024) {
		_release = false;
		Release();
		if (AllocConsole()) {
			Adjust(minLength);
			_release = Redirect();
		}
	}
	~Console() {
		if (_release)
			Release();
	}

protected:
	static void Reopen(const char* path, const char* mode, FILE* st) {
		FILE* fp;
		if (!freopen_s(&fp, path, mode, st))
			setvbuf(st, NULL, _IONBF, 0);
	}

	void Release() {
		Reopen("NUL:", "r", stdin);
		Reopen("NUL:", "w", stdout);
		Reopen("NUL:", "w", stderr);
		FreeConsole();
	}

	bool Redirect() {
		bool result = true;
		if (GetStdHandle(STD_INPUT_HANDLE) != INVALID_HANDLE_VALUE)
			Reopen("CONIN$", "r", stdin);
		if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE)
			Reopen("CONOUT$", "w", stdout);
		if (GetStdHandle(STD_ERROR_HANDLE) != INVALID_HANDLE_VALUE)
			Reopen("CONOUT$", "w", stderr);
		std::ios::sync_with_stdio(true);
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
		return result;
	}

	void Adjust(short minLength) {
		CONSOLE_SCREEN_BUFFER_INFO conInfo;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &conInfo);
		if (conInfo.dwSize.Y < minLength)
			conInfo.dwSize.Y = minLength;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), conInfo.dwSize);
	}

private:
	bool _release;
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	Console cons;
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MainWnd wnd;
	Instance inst(hInstance, &wnd);
	inst.Init(IDC_PENTAMINO, nCmdShow);
	return inst.Run();
}
