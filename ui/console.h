#pragma once

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
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
