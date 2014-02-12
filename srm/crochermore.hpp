#pragma once

#include "util.hpp"

#include <cstddef>
#include <algorithm>

// Crochermore's algorithms for exact string match reporting and string period
// computation.

namespace srm {

/// Compute the period of string X given by random-access iterator range
/// [x_begin, x_end).
///
/// The characters should be comparable with operators < and ==.
/// Integer type Idx should be large enough to hold the sizes of strings X and Y.
///
/// The algorithm used is the "SMALLEST_PERIOD" algorithm described in:
/// M. Crochemore. String-matching on ordered alphabets. Theoretical Computer Science,
/// 92:33–47, 1992.
///
/// The algorithm runs in linear time and constant space.
template <typename XI, typename Idx = std::size_t>
Idx computeStringPeriod(XI x_begin, XI x_end) {
	// Convenience function to index X.
	auto X = [x_begin](Idx i) { return *(x_begin + i); };
	Idx n = (Idx)(x_end - x_begin);
	
	Idx per = 1;
	Idx m = 0;
	MSTuple<Idx> ms{0, 1, 1};
	
	while(per + m < n) {
		if(X(per + m) == X(m)) {
			++m;
		} else {
			auto S = [&X, per, m](Idx pos) {
				if(pos == m) return X(per + m);
				return X(pos);
			};
			
			while(ms.l <= m) {
				ms = updateMS<decltype(S), Idx>(S, ms);
			}
			
			bool match = true;
			for(Idx t = 0; t < ms.s; ++t) {
				if(X(t) != S(ms.p + t)) {
					match = false;
					break;
				}
			}
			
			if(match) {
				if(ms.l - ms.s - ms.p >= ms.p) {
					per += ms.p;
					m -= ms.p - 1;
					ms.l -= ms.p;
				} else {
					per += ms.p;
					m -= ms.p - 1;
					ms = MSTuple<Idx>{0, 1, 1};
				}
			} else {
				Idx a = ms.s + ms.p * ((ms.l - ms.s) / ms.p);
				per += std::max(ms.s, std::min(m + 1 - ms.s, a)) + 1;
				m = 0;
				ms = MSTuple<Idx>{0, 1, 1};
			}
		}
	}
	
	return per;
}

/// Compute the starting positions in which string P occurs in string T.
/// Strings P and T are given as random-access iterator ranges [p_begin, p_end)
/// and [t_begin, t_end). The result indices are passed in order to function
/// output.
///
/// The characters should be comparable with operators < and ==.
/// Integer type Idx should be large enough to hold the sizes of strings P and T .
///
/// The algorithm used is the "POSITIONS" algorithm described in:
/// M. Crochemore. String-matching on ordered alphabets. Theoretical Computer Science,
/// 92:33–47, 1992.
///
/// The algorithm runs in linear time and constant space.
template <typename PI, typename TI, typename F, typename Idx = std::size_t>
void reportExactStringMatches(PI p_begin, PI p_end, TI t_begin, TI t_end, F output) {
	// Convenience functions to index P and X.
	auto P = [p_begin](Idx i) { return *(p_begin + i); };
	auto T = [t_begin](Idx i) { return *(t_begin + i); };
	Idx k = (Idx)(p_end - p_begin);
	Idx n = (Idx)(t_end - t_begin);
	
	Idx pos = 0;
	Idx m = 1;
	MSTuple<Idx> ms{0, 1, 1};
	
	while(pos <= n) {
		while(pos + m <= n && m <= k && T(pos + m - 1) == P(m - 1)) ++m;
		if(m == k + 1) output(pos);
		if(pos + m == n + 1) --m;
		
		auto S = [&P, &T, pos, m](Idx i) {
			if(i == m - 1) return T(pos + m - 1);
			return P(i);
		};
		
		while(ms.l < m) {
			ms = updateMS<decltype(S), Idx>(S, ms);
		}
		
		bool match = true;
		for(Idx t = 1; t <= ms.s; ++t) {
			if(P(t - 1) != S(ms.p + t - 1)) {
				match = false;
				break;
			}
		}
		
		if(match) {
			if(ms.l - ms.s - ms.p >= ms.p) {
				pos += ms.p;
				m -= ms.p - 1;
				ms.l -= ms.p;
			} else {
				pos += ms.p;
				m -= ms.p - 1;
				ms = MSTuple<Idx>{0, 1, 1};
			}
		} else {
			Idx a = ms.s + ms.p * ((ms.l - ms.s) / ms.p);
			pos += std::max(ms.s, std::min(m - ms.s, a)) + 1;
			m = 1;
			ms = MSTuple<Idx>{0, 1, 1};
		}
	}
}

}
