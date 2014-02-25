#include "srm/count.hpp"
#include "srm/report.hpp"
#include "srm/table.hpp"

#include "testutil.hpp"

#include <sstream>
#include <sys/time.h>
#include <climits>

// Program that reads text of length n as input and repeats the following:
// Select two a substring length s in [0, n / 2] and two substrings Y and Z,
// Y <= Z with lengths s at random. Runs different range matching algorithms
// to finds suffices of text lexicographically in [Y, Z). Prints tuple
//   - Substring length s
//   - Match count
//   - LCP of substrings
//   - Time to count
//   - Time to report
//   - Time to table
// The number of iterations can be limited by a command line argument.

/// Gets the current processor time in seconds. Linux specific, implement for
/// other platforms.
double getCPUTime() {
	timespec t;
	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t)) {
		fail("Measuring CPU time failed.");
	}
	return (double)t.tv_sec + 1e-9 * (double)t.tv_nsec;
}

class Timer {
public:
	Timer() {
		reset();
	}
	
	void reset() {
		start = getCPUTime();
	}
	
	double getElapsedTime() {
		return getCPUTime() - start;
	}
	
private:
	double start;
};

/// Simple O(n log n) suffix array construction taken from:
///   http://github.com/ttalvitie/libcontest
/// Return the start indices of the suffices of string S sorted in
/// lexicographical order. Characters past the end come before other characters
/// in the order. The empty suffix is included.
vector<int> constructSuffixArray(const string& S) {
	struct Elem {
		int start;
		int parts[2];
	};
	
	int n = S.size();
	vector<pair<char, int> > C(n + 1);
	vector<int> tmp(n + 1);
	vector<Elem> T(n + 1);
	vector<Elem> T2(n + 1);
	
	for(int i = 0; i <= n; ++i) {
		C[i].first = (i == n) ? CHAR_MIN : S[i];
		C[i].second = -i;
	}
	sort(C.begin(), C.end());
	
	for(int i = 0; i <= n; ++i) {
		int start = -C[i].second;
		T[i].start = start;
		if(start == n) {
			T[i].parts[0] = 0;
		} else {
			T[i].parts[0] = 1;
			T[i].parts[1] = S[start];
		}
	}
	
	int t = 1;
	while(t <= n) {
		for(int i = 0; i <= n; ++i) {
			if(i != 0 &&
				T[i].parts[0] == T[i - 1].parts[0] &&
				T[i].parts[1] == T[i - 1].parts[1]
			) {
				tmp[T[i].start] = tmp[T[i - 1].start];
			} else {
				tmp[T[i].start] = i;
			}
		}
		
		for(int i = 0; i <= n; ++i) {
			T[i].start = i;
			T[i].parts[0] = (i + t > n) ? 0 : tmp[i + t];
			T[i].parts[1] = tmp[i];
		}
		
		for(int s = 0; s < 2; ++s) {
			fill(tmp.begin(), tmp.end(), 0);
			for(int i = 0; i <= n; ++i) {
				++tmp[T[i].parts[s]];
			}
			
			int x = 0;
			for(int i = 0; i <= n; ++i) {
				int y = tmp[i];
				tmp[i] = x;
				x += y;
			}
			
			for(int i = 0; i <= n; ++i) {
				T2[tmp[T[i].parts[s]]++] = T[i];
			}
			
			swap(T, T2);
		}
		
		t *= 2;
	}
	
	for(int i = 0; i <= n; ++i) {
		tmp[i] = T[i].start;
	}
	
	return tmp;
}

int main(int argc, char* argv[]) {
	// Read limit from command line.
	int64_t limit = -1;
	if(argc > 1) {
		stringstream ss(argv[1]);
		ss >> limit;
		if(ss.fail() || ss.bad() || !ss.eof() || argc > 2) {
			fail("Usage: ./benchmark [iteration limit].");
		}
	}
	
	// Read input text.
	string text;
	char buf[4096];
	while(cin.good()) {
		cin.read(buf, 4096);
		text.append(buf, cin.gcount());
	}
	if(cin.bad() || !cin.eof()) fail("Reading input failed.");
	
	size_t n = text.size();
	if(n == 0) fail("Empty text is not supported.");
	
	size_t log_n = 0;
	while(((size_t)1 << log_n) < n) ++log_n;
	
	for(int64_t i = 0; i != limit; ++i) {
		size_t s = logrand(n);
		
		size_t a = rand((size_t)0, n - s);
		size_t b = rand((size_t)0, n - s);
		
		auto y_begin = text.begin() + a;
		auto y_end = y_begin + s;
		auto z_begin = text.begin() + b;
		auto z_end = z_begin + s;
		
		// Make sure that Y <= Z.
		if(lexicographical_compare(z_begin, z_end, y_begin, y_end)) {
			swap(a, b);
			swap(y_begin, z_begin);
			swap(y_end, z_end);
		}
		
		// Compute the LCP.
		size_t lcp = 0;
		while(lcp < s && *(y_begin + lcp) == *(z_begin + lcp)) ++lcp;
		
		Timer timer;
		
		// Try all the range matching algorithms, saving the results.
		
		// count
		timer.reset();
		
		size_t count_result =
			srm::makeRangeCounter(y_begin, y_end, z_begin, z_end)
				.count(text.begin(), text.end());
		
		double count_time = timer.getElapsedTime();
		
		// report
		vector<size_t> report_result;
		report_result.reserve(n);
		
		timer.reset();
		
		srm::reportRangeMatches(
			text.begin(), text.end(),
			y_begin, y_end,
			z_begin, z_end,
			[&](size_t i) { report_result.push_back(i); }
		);
		
		double report_time = timer.getElapsedTime();
		
		// table
		vector<bool> table_result(n);
		
		timer.reset();
		
		srm::computeRangeMatchTableToIterator(
			text.begin(), text.end(),
			y_begin, y_end,
			z_begin, z_end,
			table_result.begin()
		);
		
		double table_time = timer.getElapsedTime();
		
		// Cross-check results for validity, so that we get additional tests
		// and the compiler cannot realistically optimize things out.
		if(count_result != report_result.size()) {
			fail("Count and report disagree about the match count.");
		}
		
		for(size_t i : report_result) {
			if(table_result[i] == false) fail("Report and table disagree about a match.");
			table_result[i] = false;
		}
		for(size_t i = 0; i < n; ++i) {
			if(table_result[i] == true) fail("Report and table disagree about a match.");
		}
		
		cout << s << " " << count_result << " " << lcp << " " << count_time << " " << report_time << " " << table_time << "\n";
	}
	
	return 0;
}
