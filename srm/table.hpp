#pragma once

#include "util.hpp"

#include <algorithm>
#include <vector>
#include <iostream>
#include <cassert>

// Algorithm for creating lookup table of string range matches.

namespace srm {

/// Computes a boolean vector which determines for each suffix of string X
/// whether it is lexicographically less than given string Y.
/// String X and Y are given as random-access iterator ranges [x_begin, x_end)
/// and [y_begin, y_end). The output is a bitvector of the same length as X,
/// and it is written in order using the supplied functions write_output and
/// copy_output, where:
///  - set_output(Idx i, bool val) sets the value at i to val
///  - copy_output(Idx i, Idx j, Idx s) copies the values from [j, j + s),
///    to [i, i + s). The ranges are guaranteed to be disjoint.
/// It is guaranteed that all values will be filled explicitly.
///
/// The characters should be comparable with operators < and ==.
/// Integer type Idx should be large enough to hold the sizes of strings X and Y.
///
/// The algorithm used is the "Linear Time and Constant Extra Space, Copying Output"
/// algorithm described in:
/// J. Kärkkäinen, D. Kempa, S. Puglisi: String Range Matching. 2014.
template <
	typename XI, typename YI,
	typename F1, typename F2,
	typename Idx = std::size_t
>
void computeLessThanMatchTable(
	XI x_begin, XI x_end,
	YI y_begin, YI y_end,
	F1& set_output, F2& copy_output
) {
	// Convenience functions to index X and Y.
	auto X = [x_begin](Idx i) { return *(x_begin + i); };
	auto Y = [y_begin](Idx i) { return *(y_begin + i); };
	Idx n = (Idx)(x_end - x_begin);
	Idx m = (Idx)(y_end - y_begin);
	
	Idx i = 0;
	MSTuple<Idx> ms{0, 0, 0};
	Idx i_max = 0;
	MSTuple<Idx> ms_max{0, 0, 0};
	
	while(i < n) {
		while(i + ms.l < n && ms.l < m && X(i + ms.l) == Y(ms.l)) {
			ms = updateMS(y_begin, ms);
		}
		set_output(i, ms.l < m && (i + ms.l == n || X(i + ms.l) < Y(ms.l)));
		Idx j = i_max;
		if(ms.l > ms_max.l) {
			std::swap(ms, ms_max);
			i_max = i;
		}
		if(
			ms.p > 0 &&
			ms.p <= ms.l / 3 &&
			std::equal(y_begin, y_begin + ms.s, y_begin + ms.p)
		) {
			copy_output(i + 1, j + 1, ms.p - 1);
			i += ms.p;
			ms.l -= ms.p;
		} else {
			Idx h = ms.l / 3 + 1;
			copy_output(i + 1, j + 1, h - 1);
			i += h;
			ms = MSTuple<Idx>{0, 0, 0};
		}
	}
}

/// Compute the same boolean vector as computeLessThanMatchTable to random-access
/// iterator range [b_begin, b_begin + n) where n is the length of string X. Uses
/// std::copy for copying ranges. See computeLessThanMatchTable for description
/// of other parameters.
template <typename XI, typename YI, typename BI, typename Idx = std::size_t>
void computeLessThanMatchTableToIterator(
	XI x_begin, XI x_end,
	YI y_begin, YI y_end,
	BI b_begin
) {
	auto set_output = [b_begin](Idx i, bool val) {
		*(b_begin + i) = val;
	};
	auto copy_output = [b_begin](Idx i, Idx j, Idx s) {
		std::copy(b_begin + j, b_begin + j + s, b_begin + i);
	};
	computeLessThanMatchTable(
		x_begin, x_end,
		y_begin, y_end,
		set_output, copy_output
	);
}

/// Same as computeLessThanMatchTable, but instead of computing suffices of
/// X lexicographically less than Y, computes the lookup table for suffices
/// lexicographically in range [Y, Z). String Z is given as random-access
/// iterator range [z_begin, z_end).
///
/// Compared to computeLessThanMatchTable, a new function is required:
///  - xor_output(Idx i, bool val) xors value at i with val.
///
/// String Y is assumed to be lexicographically less than or equal to string Z.
/// See the documentation of computeLessThanMatchTable for more details.
template <
	typename XI, typename YI, typename ZI,
	typename F1, typename F2, typename F3,
	typename Idx = std::size_t
>
void computeRangeMatchTable(
	XI x_begin, XI x_end,
	YI y_begin, YI y_end,
	ZI z_begin, ZI z_end,
	F1& set_output, F2& copy_output, F3& xor_output
) {
	computeLessThanMatchTable(
		x_begin, x_end,
		z_begin, z_end,
		set_output, copy_output
	);
	computeLessThanMatchTable(
		x_begin, x_end,
		y_begin, y_end,
		xor_output, copy_output
	);
}

/// Compute the same boolean vector as computeRangeMatchTable to random-access
/// iterator range [b_begin, b_begin + n) where n is the length of string X. Uses
/// std::copy for copying ranges. See computeRangeMatchTable for description
/// of other parameters.
template <
	typename XI, typename YI, typename ZI,
	typename BI,
	typename Idx = std::size_t
>
void computeRangeMatchTableToIterator(
	XI x_begin, XI x_end,
	YI y_begin, YI y_end,
	ZI z_begin, YI z_end,
	BI b_begin
) {
	// TODO: use constant extra space?
	Idx n = (Idx)(x_end - x_begin);
	
	std::vector<char> a(n);
	std::vector<char> b(n);
	
	computeLessThanMatchTableToIterator(x_begin, x_end, y_begin, y_end, a.begin());
	computeLessThanMatchTableToIterator(x_begin, x_end, z_begin, z_end, b.begin());
	
	for(Idx i = 0; i < n; ++i) {
		*(b_begin + i) = a[i] != b[i];
	}
}

}
