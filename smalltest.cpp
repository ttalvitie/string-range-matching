#include "srm/table.hpp"
#include "srm/count.hpp"
#include "srm/report.hpp"

#include "testutil.hpp"

#include <iostream>
#include <vector>
#include <array>

// Range matching tests that exhaustively try all 0-6 character strings with
// alphabet 0-2 / A-C.

using namespace std;

struct String {
	String(int size, int bits) : size(size) {
		chars[0] = bits & 3;
		chars[1] = (bits >> 2) & 3;
		chars[2] = (bits >> 4) & 3;
		chars[3] = (bits >> 6) & 3;
		chars[4] = (bits >> 8) & 3;
	}
	
	const char* begin() const {
		return &chars[0];
	}
	char* begin() {
		return &chars[0];
	}
	const char* end() const {
		return begin() + size;
	}
	char* end() {
		return begin() + size;
	}
	
	char chars[7];
	int size;
};

// For printing strings in debugging.
ostream& operator<<(ostream& out, const String& s) {
	for(char c : s) {
		out << (char)('A' + c);
	}
	return out;
}

void testRangeMatch(String X, String Y, String Z) {
	vector<int> reports;
	srm::reportRangeMatches(
		X.begin(), X.end(),
		Y.begin(), Y.end(),
		Z.begin(), Z.end(),
		[&](int i) { reports.push_back(i); }
	);
	
	int count = srm::makeRangeCounter(Y.begin(), Y.end(), Z.begin(), Z.end())
		.count(X.begin(), X.end());
	
	vector<bool> table(X.size);
	srm::computeRangeMatchTableToIterator(
		X.begin(), X.end(),
		Y.begin(), Y.end(),
		Z.begin(), Z.end(),
		table.begin()
	);
	
	// Cross-check all three.
	if((int)reports.size() != count) fail("report and count disagree.");
	
	vector<int> table_reports;
	for(int i = 0; i < X.size; ++i) {
		if(table[i]) {
			table_reports.push_back(i);
		}
	}
	
	sort(reports.begin(), reports.end());
	if(reports.size() != table_reports.size()) fail("report and table disagree.");
	if(!equal(reports.begin(), reports.end(), table_reports.begin())) fail("report and table disagree.");
	
	// Compare table to a trivial implementation.
	char* it = X.begin();
	for(int i = 0; i < X.size; ++i) {
		bool match =
			!lexicographical_compare(it, X.end(), Y.begin(), Y.end()) &&
			lexicographical_compare(it, X.end(), Z.begin(), Z.end());
		
		if(table[i] != match) fail("table and trivial implementation disagree.");
		
		++it;
	}
}

int main() {
	// Try all pairs X, Y, Z.
	int done = 0;
	for(int Xs = 0; Xs <= 6; ++Xs) {
	for(int Xb = 0; Xb < (1 << (2 * Xs)); ++Xb) {
		// Disallow 3.
		if(Xb & (Xb >> 1) & 0x555) continue;
		
		String X(Xs, Xb);
		
		for(int Ys = 0; Ys <= 6; ++Ys) {
		for(int Yb = 0; Yb < (1 << (2 * Ys)); ++Yb) {
			if(Yb & (Yb >> 1) & 0x555) continue;
			
			String Y(Ys, Yb);
			for(int Zs = 0; Zs <= 6; ++Zs) {
			for(int Zb = 0; Zb < (1 << (2 * Zs)); ++Zb) {
				if(Zb & (Zb >> 1) & 0x555) continue;
				
				String Z(Zs, Zb);
				
				// We require that Y <= Z.
				if(!lexicographical_compare(Y.begin(), Y.end(), Z.begin(), Z.end())) continue;
				
				testRangeMatch(X, Y, Z);
				
				++done;
			}
			}
		}
		}
		
		cout << (double)done / 6588669.76 << "% done.\n";
	}
	}
	
	cout << "All OK!\n";
	
	return 0;
}
