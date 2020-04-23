// pentatest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <io.h>

#include "../pentalib/figure.h"
#include "../ui/elapsed.h"

double test(const char* name, int& nsol) {
	Field field;
	field.Load(name);
	Solution sol;
	Context cntx{sol, true};
	Elapsed el;
	field.SolveMT(cntx);
	double sec = el.sec();
	nsol = (int)sol.size();
	printf("%s\t%d msec\t%d solutions\t%lld recursions\n",
		name, (int)(sec * 1000), nsol, cntx.dbgcnt);
	return sec;
}

int main(int argc, char *argv[]) {
	auto ti = time(nullptr);
	printf("%s", ctime(&ti));
	printf("test begin\n");

	double tsec = 0;
	int tcnt = 0, tfail = 0;
	char path[1024];

	_finddata_t fd;
	auto h = _findfirst("../figures/*.pnt", &fd);
	if (h != -1) {
		do{
			strcpy(path, "../figures/");
			strcat(path, fd.name);
			int nsol = 0;
			tsec += test(path, nsol);
			tcnt++;
			if (!nsol)
				tfail++;
		} while (0 == _findnext(h, &fd));
		_findclose(h);
	}

	printf("total: %8.3f sec, %d failed, %d tests\n", tsec, tfail, tcnt);
	printf("test end\n\n");
	return 0;
}
