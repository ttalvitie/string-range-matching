#pragma once

#include "util.hpp"
#include "crochermore.hpp"

#include <cstddef>
#include <algorithm>

// Algorithm for creating listing string range matches.

namespace srm {

/// Given strings X and Y, finds the suffixes of X that are lexicographically
/// in range [Y', Y), where Y' is a prefix of Y. Strings X, Y and Y' are given
/// as random-access iterator ranges [x_begin, x_end), [y_begin, y_end) and
/// [y_begin, yp_end). The starting indices of the matches in X are passed to
/// function output. The output indices are not in order but are unique.
/// 
/// The characters should be comparable with operators < and ==.
/// Integer type Idx should be large enough to hold two times the sizes of
/// strings X and Y.
///
/// The algorithm used is the restricted case of the "O(n log(m1 + m2)) Time and
/// Constant Extra Space" algorithm described in:
/// J. Kärkkäinen, D. Kempa, S. Puglisi: String Range Matching. 2014.
///
/// The algorithm runs in O(|X| log(|Y| / |Y'|)) time and uses constant space.
template <typename XI, typename YI, typename F, typename Idx = std::size_t>
void reportRestrictedRangeMatches(
	XI x_begin, XI x_end,
	YI y_begin, YI y_end, YI yp_end,
	F output
) {
	// Convenience functions to index X and Y.
	auto X = [x_begin](Idx i) { return *(x_begin + i); };
	auto Y = [y_begin](Idx i) { return *(y_begin + i); };
	Idx n = (Idx)(x_end - x_begin);
	Idx M = (Idx)(y_end - y_begin);
	Idx R = (Idx)(yp_end - y_begin);
	
	// Split [Y[0..R), Y(0..M)) into disjoint subsets [Y[0..r), Y(0..m))
	// where r >= floor(2m/3).
	Idx r = R;
	while(r < M) {
		Idx m = std::min(r + r / 2 + 1, M);
		
		Idx i = 0;
		MSTuple<Idx> ms{0, 0, 0};
		
		Idx q = computeStringPeriod(y_begin, y_begin + r);
		Idx e = 0;
		while(q + e < m && Y(e) == Y(q + e)) ++e;
		e += q;
		
		while(i < n) {
			while(i + ms.l < n && ms.l < m && X(i + ms.l) == Y(ms.l)) {
				ms = updateMS<decltype(Y), Idx>(Y, ms);
			}
			
			if(r <= ms.l && ms.l < m && (i + ms.l == n || X(i + ms.l) < Y(ms.l))) output(i);
			
			Idx h;
			if(
				ms.p > 0 && ms.p <= ms.l / 3 &&
				std::equal(y_begin, y_begin + ms.s, y_begin + ms.p)
			) {
				h = ms.p;
				ms.l -= ms.p;
			} else {
				h = ms.l / 3 + 1;
				ms = MSTuple<Idx>{0, 0, 0};
			}
			
			if(e < m && Y(e) < Y(e % q)) {
				Idx g = (e - r) / q;
				Idx x = i;
				for(Idx t = 0; t < g; ++t) {
					x += q;
					if(x >= i + h) break;
					output(x);
				}
			}
			
			i += h;
		}
		
		r = m;
	}
}

}
