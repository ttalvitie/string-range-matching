#pragma once

#include <vector>

// Algorithms for counting the number of string range matches.

namespace srm {

/// Workspace for counting the number of suffices of string X that are
/// lexicographically smaller than the constant string Y.
/// The characters should be comparable with operators < and ==.
/// Integer type Idx should be large enough to hold the sizes of strings
/// X and Y times the parameter k given in constructor.
template <typename YI, typename Idx = std::size_t>
class OneSidedCounter {
public:
	/// Construct the workspace for Y given by random-access iterator range
	/// [y_begin, y_end).
	/// String Y must stay constant throughout the lifetime of the workspace.
	/// Parameter k >= 3 is the parameter for the algorithm described in TODO: source.
	OneSidedCounter(YI y_begin, YI y_end, Idx k);
	
	/// Return the count of suffices of X lexicographically smaller than Y.
	/// String X is given by random-access iterator range [x_begin, x_end).
	template <typename XI>
	Idx count(XI x_begin, XI x_end);
	
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
	SpElement findSp(Idx x);
	
	/// Returns the element in Sn such that b <= x and b is as large as
	/// possible. Assumes that such an element exists.
	SnElement predSn(Idx x);
};

}

#include "count_impl.hpp"
