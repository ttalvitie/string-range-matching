#pragma once

#include <algorithm>

namespace srm {

template <typename YI, typename Idx>
OneSidedCounter<YI, Idx>::OneSidedCounter(YI y_begin, YI y_end, Idx k)
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

template <typename YI, typename Idx>
template <typename XI>
Idx OneSidedCounter<YI, Idx>::count(XI x_begin, XI x_end) {
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

template <typename YI, typename Idx>
typename OneSidedCounter<YI, Idx>::SpElement OneSidedCounter<YI, Idx>::findSp(Idx x) {
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

template <typename YI, typename Idx>
typename OneSidedCounter<YI, Idx>::SnElement OneSidedCounter<YI, Idx>::predSn(Idx x) {
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


}
