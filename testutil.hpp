#pragma once

#include <random>
#include <algorithm>
#include <iostream>
#include <cmath>

using namespace std;

mt19937 rng(std::random_device{}());

/// Return uniform random integer from range [a, b] u [b, a].
template <typename T>
T rand(T a, T b) {
	if(a > b) swap(a, b);
	return uniform_int_distribution<T>(a, b)(rng);
}

/// Pick x from range [0, log(n + 1)) and return floor(exp(x)). Result is in
/// range [1, n].
int logrand(int n) {
	assert(n > 0);
	double x = uniform_real_distribution<double>(0, log(n + 1))(rng);
	return max(1, min(n, (int)floor(exp(x))));
}

template <typename R>
R choice_(size_t ind) {
	return R();
}

template <typename R, typename FT, typename... T>
R choice_(size_t ind, const FT& fv, const T&... v) {
	if(ind == 0) return fv;
	return choice_<R, T...>(ind - 1, v...);
}

/// Return random choice of the parameters.
template <typename FT, typename... T>
FT choice(const FT& fv, const T&... v) {
	return choice_<FT, FT, T...>(rand((size_t)0, sizeof...(v)), fv, v...);
}

/// Return random vector of given length of elements from between a and b.
template <typename T>
vector<T> randvec(size_t length, T a, T b) {
	vector<T> ret(length);
	for(size_t i = 0; i < length; ++i) {
		ret[i] = rand(a, b);
	}
	return ret;
}

/// Return random string of given length of characters from between a and b.
string randstring(size_t length, char a, char b) {
	string ret(length, ' ');
	for(size_t i = 0; i < length; ++i) {
		ret[i] = (char)rand((int)a, (int)b);
	}
	return ret;
}

static void fail_() {
	cerr << "\n";
	abort();
}

template <typename FirstT, typename... T>
void fail_(FirstT first, T... msg) {
	cerr << first;
	fail_(msg...);
}

/// Abort with error message given in parameters.
template <typename... T>
void fail(T... msg) {
	cerr << "FAIL: ";
	fail_(msg...);
}
