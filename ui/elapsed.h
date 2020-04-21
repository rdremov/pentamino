#pragma once
#include <chrono>

class Elapsed {
	using clock = std::chrono::steady_clock;
	clock::time_point _1, _2;

public:
	Elapsed() {
		_1 = clock::now();
	}
	~Elapsed() {}

	double sec() {
		_2 = clock::now();
		return std::chrono::duration<double>(_2 - _1).count();
	}
};

