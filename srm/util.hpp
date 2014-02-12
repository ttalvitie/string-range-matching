#pragma once

#include <cstddef>

// Utility functions used in implementing string range matching algorithms.

namespace srm {

/// Tuple containing the starting index and period of the lexicographically
/// maximal suffix of a prefix of a string Y.
/// Idx is the integer type used for the indices.
template <typename Idx>
struct MSTuple {
	Idx l; ///< Length of the prefix of the string Y.
	Idx s; ///< Starting index of the maximal suffix of Y[0, l).
	Idx p; ///< Period of the maximal suffix of Y[0, l).
};

/// For string Y given by zero-indexed character access function Y and a
/// corresponding MS tuple ms, find the MS tuple with l increased by one.
/// It is assumed that l is less than the length of Y.
/// The characters should be comparable with operators < and ==.
/// Idx should be sufficient to hold the length of string Y.
///
/// The algorithm used is the UpdateMS function described in:
/// J. Kärkkäinen, D. Kempa, S. Puglisi: Crochermore's String Matching Algorithm,
/// Simplification, Extensions, Applications. In Proc. PSC 2013, pages 168-175,
/// Czech Technical university, 2013.
///
/// The algorithm runs in O(change in s) time.
template <typename F, typename Idx>
MSTuple<Idx> updateMS(F Y, MSTuple<Idx> ms) {
	if(ms.l == 0) return MSTuple<Idx>{1, 0, 1};
	
	Idx i = ms.l;
	while(i <= ms.l) {
		if(Y(i) < Y(i - ms.p)) {
			i -= (i - ms.s) % ms.p;
			ms.s = i;
			ms.p = 1;
		} else if(!(Y(i) == Y(i - ms.p))) {
			ms.p = i - ms.s + 1;
		}
		++i;
	}
	
	++ms.l;
	
	return ms;
}

}
