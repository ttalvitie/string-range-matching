#pragma once

#include <vector>
#include <algorithm>

// Algorithms for counting the number of string range matches.

namespace srm {

/// Workspace for counting the number of suffices of string X that are
/// lexicographically smaller than the constant string Y.
/// String Y must stay constant throughout the lifetime of the workspace.
/// The characters should be comparable with operators < and ==.
/// Integer type Idx should be large enough to hold the sizes of strings
/// X and Y times the parameter k given in constructor.
template <typename YI, typename Idx = std::size_t>
class LessThanCounter {
public:
	/// Construct the workspace for Y given by random-access iterator range
	/// [y_begin, y_end).
	/// Parameter k >= 3 is the parameter for the algorithm described in TODO: source.
	LessThanCounter(YI y_begin, YI y_end, Idx k = 3)
		: y_begin(y_begin),
		  y_end(y_end),
		  k(k)
	{
		/// TODO: k validity checking?
		
		// Convenience function to index Y.
		auto Y = [y_begin](Idx i) { return *(y_begin + i); };
		Idx m = (Idx)(y_end - y_begin);
		
		// Precompute Sp and Sn.
		Sn.push_back(SnElement{1, 0});
		
		Idx i = 1;
		Idx last = 1;
		Idx l = 0;
		Idx count = 0;
		while(i < m) {
			while(i + l < m && Y(i + l) == Y(l)) ++l;
			
			SpElement found = findSp(l);
			Idx b = found.b;
			Idx e = found.e;
			Idx c = found.c;
			
			if(b == 0 && (k - 1) * i <= l) {
				b = 2 * i;
				e = i + l;
				c = count;
				Sp.push_back(SpElement{b, e, c});
			}
			if(2 * last <= i) {
				Sn.push_back(SnElement{i, count});
				last = i;
			}
			if(i + l == m || Y(i + l) < Y(l)) ++count;
			if(b != 0) {
				count += c;
				i += b / 2;
				l -= b / 2;
			} else {
				SnElement pred = predSn(l / k + 1);
				b = pred.b;
				c = pred.c;
				
				count += c;
				i += b;
				l = 0;
			}
		}
	}
	
	/// Return the count of suffices of X lexicographically smaller than Y.
	/// String X is given by random-access iterator range [x_begin, x_end).
	template <typename XI>
	Idx count(XI x_begin, XI x_end) {
		// Convenience functions to index X and Y.
		auto X = [x_begin](Idx i) { return *(x_begin + i); };
		auto Y = [this](Idx i) { return *(y_begin + i); };
		Idx n = (Idx)(x_end - x_begin);
		Idx m = (Idx)(y_end - y_begin);
		
		Idx count = 0;
		Idx i = 0;
		Idx l = 0;
		
		while(i < n) {
			while(i + l < n && l < m && X(i + l) == Y(l)) ++l;
			
			SpElement found = findSp(l);
			Idx b = found.b;
			Idx c = found.c;
			
			if(l < m && (i + l == n || X(i + l) < Y(l))) ++count;
			if(b != 0) {
				count += c;
				i += b / 2;
				l -= b / 2;
			} else {
				SnElement pred = predSn(l / k + 1);
				b = pred.b;
				c = pred.c;
				count += c;
				i += b;
				l = 0;
			}
		}
		
		return count;
	}
	
private:
	YI y_begin;
	YI y_end;
	Idx k;
	
	struct SpElement {
		Idx b;
		Idx e;
		Idx c;
	};
	struct SnElement {
		Idx b;
		Idx c;
	};
	std::vector<SpElement> Sp; ///< List Sp precomputed for Y, sorted by b.
	std::vector<SnElement> Sn; ///< List Sn precomputed for Y, sorted by b.
	
	/// Returns the element in Sp such that b <= x < e, or if none is found,
	/// returns {0, 0, 0}.
	SpElement findSp(Idx x) {
		// Find the element using binary search.
		SpElement elem{x, 0, 0};
		auto cmp = [](const SpElement& a, const SpElement& b) {
			return a.b < b.b;
		};
		auto it = std::upper_bound(Sp.begin(), Sp.end(), elem, cmp);
		
		// Now 'it' contains the first Sp element with b > x.
		if(it == Sp.begin()) return SpElement{0, 0, 0};
		--it;
		if(it->e <= x) return SpElement{0, 0, 0};
		return *it;
	}
	
	/// Returns the element in Sn such that b <= x and b is as large as
	/// possible. Assumes that such an element exists.
	SnElement predSn(Idx x) {
		// Find the element using binary search.
		SnElement elem{x, 0};
		auto cmp = [](const SnElement& a, const SnElement& b) {
			return a.b < b.b;
		};
		auto it = std::upper_bound(Sn.begin(), Sn.end(), elem, cmp);
		
		// Now 'it' contains the first Sp element with b > x.
		--it;
		return *it;
	}
};

/// Equivalent to constructor of LessThanCounter of appropriate type.
template <typename YI, typename Idx = std::size_t>
LessThanCounter<YI, Idx> makeLessThanCounter(YI y_begin, YI y_end, Idx k = 3) {
	return LessThanCounter<YI, Idx>(y_begin, y_end, k);
}

/// Workspace for counting the number of suffices of string X that are
/// lexicographically between the constant string Y and Z, i.e. in range [Y, Z).
/// Strings Y and Z must stay constant throughout the lifetime of the workspace.
/// Y is assumed to be lexicographically less than or equal to Z.
/// The characters should be comparable with operators < and ==.
/// Integer type Idx should be large enough to hold the sizes of strings
/// X, Y and Z times the parameter k given in constructor.
template <typename YI, typename ZI, typename Idx = std::size_t>
class RangeCounter {
public:
	/// Construct the workspace for Y and Z given by random-access iterator ranges
	/// [y_begin, y_end) and [z_begin, z_end[.
	/// Parameters ky, kz >= 3 are the parameters for the algorithm described in TODO: source,
	/// ky is used for bound Y and kz for bound Z.
	RangeCounter(
		YI y_begin, YI y_end,
		ZI z_begin, ZI z_end,
		Idx ky = 3, Idx kz = 3
	)
		: y_counter(y_begin, y_end, ky),
		  z_counter(z_begin, z_end, kz)
	{ }
	
	/// Return the count of suffices of X lexicographically in range [Y, Z).
	/// String X is given by random-access iterator range [x_begin, x_end).
	template <typename XI>
	Idx count(XI x_begin, XI x_end) {
		return z_counter.count(x_begin, x_end) - y_counter.count(x_begin, x_end);
	}
	
private:
	LessThanCounter<YI, Idx> y_counter; /// Counter for range ["", Y).
	LessThanCounter<ZI, Idx> z_counter; /// Counter for range ["", Z).
};

/// Equivalent to constructor of RangeCounter of appropriate type.
template <typename YI, typename ZI, typename Idx = std::size_t>
RangeCounter<YI, ZI, Idx> makeRangeCounter(
	YI y_begin, YI y_end,
	ZI z_begin, ZI z_end,
	Idx ky = 3, Idx kz = 3
) {
	return RangeCounter<YI, ZI, Idx>(y_begin, y_end, z_begin, z_end, ky, kz);
}

}
