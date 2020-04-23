// pentatest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <io.h>

#include "../pentalib/figure.h"
#include "../ui/elapsed.h"

int test(const char* name) {
	Field field;
	field.Load(name);
	Solution sol;
	Context cntx{sol};
	Elapsed el;
	field.Solve(cntx);
	int msec = (int) (1000 * el.sec());
	printf("%s time = %d msec sol=%d dbgcnt = %lld\n",
		name, msec, (int)sol.size(), cntx.dbgcnt);
	return msec;
}

int main(int argc, char *argv[]) {
	printf("pentatest begin\n");

	auto ti = time(nullptr);
	printf("%s\n", ctime(&ti));

	char path[1024];
	_finddata_t fd;
	auto h = _findfirst("../figures/*.pnt", &fd);
	if (h != -1) {
		do{
			strcpy(path, "../figures/");
			strcat(path, fd.name);
			test(path);
		} while (0 == _findnext(h, &fd));
		_findclose(h);
	}

	printf("pentatest end\n\n");
	return 0;
}
