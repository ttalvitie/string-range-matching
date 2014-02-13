#include "srm/table.hpp"
#include "srm/count.hpp"
#include "srm/crochermore.hpp"
#include "srm/report.hpp"

#include "testutil.hpp"

#include <iostream>
#include <vector>
#include <random>

using namespace std;

void randomTestLessThanMatch() {
	int a = rand(0, choice(3, 8, 20));
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
	int a = rand(0, choice(3, 8, 20));
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
	int a = rand(0, choice(3, 8, 20));
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

void randomTestExactStringMatching() {
	int a = rand(0, choice(3, 8, 20));
	string P = randstring(rand(0, choice(5, 15, 30)), 'A', 'A' + a);
	
	int prob = rand(1, choice(5, 20, 100));
	
	string T;
	int len = rand(0, 500);
	while((int)T.size() < len) {
		if(rand(0, prob) == 0) {
			T.append(P);
		} else {
			T.append(randstring(1, 'A', 'A' + a));
		}
	}
	
	vector<int> matches;
	for(int i = 0; i + (int)P.size() <= (int)T.size(); ++i) {
		bool match = true;
		for(int j = 0; j < (int)P.size(); ++j) {
			if(T[i + j] != P[j]) {
				match = false;
				break;
			}
		}
		if(match) matches.push_back(i);
	}
	
	vector<int> cmpmatches;
	srm::reportExactStringMatches(
		P.begin(), P.end(),
		T.begin(), T.end(),
		[&](int i) { cmpmatches.push_back(i); }
	);
	
	if(matches.size() != cmpmatches.size()) fail();
	if(!equal(matches.begin(), matches.end(), cmpmatches.begin())) fail();
}

void randomTestRestrictedRangeMatches() {
	int a = rand(0, choice(3, 8, 20));
	string X = randstring(rand(0, choice(5, 15, 30)), 'A', 'A' + a);
	string Y;
	if(X.empty() || choice(true, false)) {
		Y = randstring(rand(1, choice(5, 15, 30)), 'A', 'A' + a);
	} else {
		int i = rand(0, (int)X.size() - 1);
		Y = X.substr(i, rand(1, (int)X.size() - i));
	}
	int m = Y.size();
	int r = rand(0, m);
	
	string Yp = Y.substr(0, r);
	
	vector<int> matches;
	for(int i = 0; i < (int)X.size(); ++i) {
		string sub = X.substr(i);
		if(sub >= Yp && sub < Y) {
			matches.push_back(i);
		}
	}
	
	vector<int> cmpmatches;
	srm::reportRestrictedRangeMatches(
		X.begin(), X.end(),
		Y.begin(), Y.end(), Y.begin() + r,
		[&](int i) {
			cmpmatches.push_back(i);
		}
	);
	
	sort(cmpmatches.begin(), cmpmatches.end());
	if(matches.size() != cmpmatches.size()) fail();
	if(!equal(matches.begin(), matches.end(), cmpmatches.begin())) fail();
}

int main() {
	while(true) {
		randomTestLessThanMatch();
		randomTestRangeMatch();
		randomTestStringPeriod();
		randomTestExactStringMatching();
		randomTestRestrictedRangeMatches();
	}
	
	return 0;
}
