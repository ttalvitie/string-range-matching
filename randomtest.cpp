#include "srm/table.hpp"
#include "srm/count.hpp"
#include "srm/crochermore.hpp"

#include "testutil.hpp"

#include <iostream>
#include <vector>
#include <random>

using namespace std;

void randomTestLessThanMatch() {
	int a = rand(1, choice(3, 8, 20));
	string X = randstring(rand(0, choice(5, 15)), 'A', 'A' + a);
	string Y = randstring(rand(0, choice(5, 15)), 'A', 'A' + a);
	
	vector<bool> B(X.size());
	srm::computeLessThanMatchTableToIterator(
		X.begin(), X.end(),
		Y.begin(), Y.end(),
		B.begin()
	);
	
	for(size_t i = 0; i < X.size(); ++i) {
		string Xi = X.substr(i);
		if(B[i] != (Xi < Y)) fail();
	}
	
	size_t count = 0;
	for(bool b : B) {
		count += (size_t)b;
	}
	size_t cmpcount = srm::makeLessThanCounter(Y.begin(), Y.end())
		.count(X.begin(), X.end());
	if(count != cmpcount) fail();
}

void randomTestRangeMatch() {
	int a = rand(1, choice(3, 8, 20));
	string X = randstring(rand(0, choice(5, 15)), 'A', 'A' + a);
	string Y = randstring(rand(0, choice(5, 15)), 'A', 'A' + a);
	string Z = randstring(rand(0, choice(5, 15)), 'A', 'A' + a);
	
	if(Y > Z) swap(Y, Z);
	
	vector<bool> B(X.size());
	srm::computeRangeMatchTableToIterator(
		X.begin(), X.end(),
		Y.begin(), Y.end(),
		Z.begin(), Z.end(),
		B.begin()
	);
	
	for(size_t i = 0; i < X.size(); ++i) {
		string Xi = X.substr(i);
		if(B[i] != (Xi >= Y && Xi < Z)) fail();
	}
	
	size_t count = 0;
	for(bool b : B) {
		count += (size_t)b;
	}
	size_t cmpcount = srm::makeRangeCounter(Y.begin(), Y.end(), Z.begin(), Z.end())
		.count(X.begin(), X.end());
	if(count != cmpcount) fail();
}

void randomTestStringPeriod() {
	int a = rand(1, choice(3, 8, 20));
	string X = randstring(rand(0, choice(5, 15, 30)), 'A', 'A' + a);
	int n = X.size();
	
	int period;
	for(period = 1; ; ++period) {
		bool ok = true;
		for(int i = 0; i + period < n; ++i) {
			if(X[i] != X[i + period]) {
				ok = false;
				break;
			}
		}
		if(ok) break;
	}
	
	size_t cmpperiod = srm::computeStringPeriod(X.begin(), X.end());
	if((size_t)period != cmpperiod) fail();
}

int main() {
	while(true) {
		randomTestStringPeriod();
		randomTestRangeMatch();
		randomTestLessThanMatch();
	}
	
	return 0;
}