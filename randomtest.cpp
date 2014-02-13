#include "srm/table.hpp"
#include "srm/count.hpp"
#include "srm/crochermore.hpp"
#include "srm/report.hpp"

#include "testutil.hpp"

#include <iostream>
#include <vector>

// Tests on randomly generated strings compared to naive solutions.

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
	
	for(int i = 0; i < (int)X.size(); ++i) {
		string Xi = X.substr(i);
		if(B[i] != (Xi < Y)) fail();
	}
	
	int count = 0;
	for(bool b : B) {
		count += (int)b;
	}
	int cmpcount = srm::makeLessThanCounter(Y.begin(), Y.end())
		.count(X.begin(), X.end());
	if(count != cmpcount) fail();
}

void randomTestRangeMatch() {
	int a = rand(0, choice(3, 8, 20));
	string X = randstring(rand(0, choice(5, 15)), 'A', 'A' + a);
	string Y = randstring(rand(0, choice(5, 15)), 'A', 'A' + a);
	string Z = randstring(rand(0, choice(5, 15)), 'A', 'A' + a);
	
	X.push_back('Z');
	Y.push_back('Z');
	Z.push_back('Z');
	
	if(Y > Z) swap(Y, Z);
	
	vector<bool> B(X.size());
	srm::computeRangeMatchTableToIterator(
		X.begin(), X.end(),
		Y.begin(), Y.end(),
		Z.begin(), Z.end(),
		B.begin()
	);
	
	vector<int> matches;
	for(int i = 0; i < (int)X.size(); ++i) {
		string Xi = X.substr(i);
		if(B[i] != (Xi >= Y && Xi < Z)) fail();
		if(B[i]) matches.push_back(i);
	}
	
	vector<int> cmpmatches;
	srm::reportRangeMatches(
		X.begin(), X.end(),
		Y.begin(), Y.end(),
		Z.begin(), Z.end(),
		[&](int i) {
			cmpmatches.push_back(i);
		}
	);
	
	sort(cmpmatches.begin(), cmpmatches.end());
	
	if(matches.size() != cmpmatches.size()) fail();
	if(!equal(matches.begin(), matches.end(), cmpmatches.begin())) fail();
	
	int count = 0;
	for(bool b : B) {
		count += b;
	}
	int cmpcount = srm::makeRangeCounter(Y.begin(), Y.end(), Z.begin(), Z.end())
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
	
	int cmpperiod = srm::computeStringPeriod(X.begin(), X.end());
	if(period != cmpperiod) fail();
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
	bool less_than = choice(true, false);
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
		if(sub.size() < Yp.size() || !equal(Yp.begin(), Yp.end(), sub.begin())) continue;
		if(less_than == (sub < Y)) {
			matches.push_back(i);
		}
	}
	
	vector<int> cmpmatches;
	srm::reportRestrictedRangeMatches(
		X.begin(), X.end(),
		Y.begin(), Y.end(), Y.begin() + r,
		[&](int i) {
			cmpmatches.push_back(i);
		},
		less_than
	);
	
	sort(cmpmatches.begin(), cmpmatches.end());
	
	if(matches.size() != cmpmatches.size()) fail();
	if(!equal(matches.begin(), matches.end(), cmpmatches.begin())) fail();
}

int main() {
	cout << "Starting random testing. On failure, shows FAIL. Runs infinitely.\n";
	int64_t count = 0;
	int64_t report_interval = 10000;
	while(true) {
		randomTestLessThanMatch();
		randomTestRangeMatch();
		randomTestStringPeriod();
		randomTestExactStringMatching();
		randomTestRestrictedRangeMatches();
		++count;
		if(count % report_interval == 0) cout << "Run " << count << " cycles.\n";
	}
	
	return 0;
}
