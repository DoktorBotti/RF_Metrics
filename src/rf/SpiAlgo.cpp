//
// Created by Robert Schmoltzi on 19.05.21.
//

#include "SpiAlgo.h"
#include <boost/math/special_functions/factorials.hpp>

static GeneralizedRfAlgo::Scalar aSmallerB(const size_t a, const size_t b, const size_t n) {
	return GeneralizedRfAlgo::factorials.lg_unrooted_dbl_fact_fast(static_cast<long>(n)) -
	       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(b)) -
	       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(n - a)) +
	       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(b - a + 1));
}

// Input: |A1|, |A2|, |X|
// Called if splits overlap
// Output:
// if(S1 == S2):
//      - lg_root(|A1|) - lg_root(|B1|) + lg_unroot(|X|)      [ <=> h(S1)]
// else: // Let S1 be the greater split
//      - lg_root(|A1|) - lg_root(|B2|) + lg_root(|A1| - |A2| + 1) + lg_unroot(|X|)  [ <=> h(S1,S2)]
GeneralizedRfAlgo::Scalar SpiAlgo::one_overlap(const size_t a, const size_t b, const size_t n) {
	if (a == b) {
		return GeneralizedRfAlgo::factorials.lg_unrooted_dbl_fact_fast(static_cast<long>(n)) -
		       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(a)) -
		       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(n - a));
	}
	if (a < b) {
		return aSmallerB(a, b, n);
	}

	return GeneralizedRfAlgo::factorials.lg_unrooted_dbl_fact_fast(static_cast<long>(n)) -
	       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(a)) -
	       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(n - b)) +
	       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(a - b + 1));
}

// Input: |A1|, |B2|, |X|
// Called if splits overlap
// Output:
// if(S1 == S2):
//      - lg_root(|A2|) - lg_root(|B2|) + lg_unroot(|X|)      [ <=> h(S1)]
// else: // Let S1 be the greater split
//      - lg_root(|A1|) - lg_root(|B2|) + lg_root(|A1| - |A2| + 1) + lg_unroot(|X|)  [ <=> h(S1,S2)]
GeneralizedRfAlgo::Scalar
SpiAlgo::one_overlap_notb(const size_t a, const size_t n_minus_b, const size_t n) {
	const size_t b = n - n_minus_b;
	if (a == b) {
		return GeneralizedRfAlgo::factorials.lg_unrooted_dbl_fact_fast(static_cast<long>(n)) -
		       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(b)) -
		       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(n_minus_b));
	}
	if (a < b) {
		return aSmallerB(a, b, n);
	}
	return GeneralizedRfAlgo::factorials.lg_unrooted_dbl_fact_fast(static_cast<long>(n)) -
	       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(a)) -
	       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(n_minus_b)) +
	       GeneralizedRfAlgo::factorials.lg_rooted_dbl_fact_fast(static_cast<long>(a - b + 1));
}

// Based on the implementation by Martin R. Smith, found at
// https://github.com/ms609/TreeDist/blob/e05ab4c9e69c9548f066b2a7b256e35f7f92067d/src/tree_distance_functions.cpp
GeneralizedRfAlgo::Scalar SpiAlgo::calc_split_score(const PllSplit &S1, const PllSplit &S2) {
	{
		bool found = false;
		for (size_t i = 0; i != PllSplit::split_len; ++i) {
			// TODO: Time direct access vs vectorized Split-Methods (less iterations vs doing less
			// in each iteration)
			if ((~S1()[i] & S2()[i])) {
				found = true;
				break;
			}
		}
		if (!found)
			return one_overlap(S1.getPrecalcPopcnt(), S2.getPrecalcPopcnt(), taxa);
	}
	{
		bool found = false;

		for (size_t i = 0; i != PllSplit::split_len; ++i) {
			if ((S1()[i] & ~S2()[i])) {
				found = true;
				break;
			}
		}
		if (!found)
			return one_overlap(S1.getPrecalcPopcnt(), S2.getPrecalcPopcnt(), taxa);
	}
	const size_t bits_too_many = GeneralizedRfAlgo::bits_too_many(taxa);
	for (size_t i = 0; i != PllSplit::split_len; ++i) {
		pll_split_base_t test = ~(S1()[i] | S2()[i]);
		if (i == PllSplit::split_len - 1 && bits_too_many) {
			test &= (static_cast<pll_split_base_t>(~0) >> bits_too_many);
		}
		if (test) {
			return 0;
		}
	}
	return one_overlap_notb(S1.getPrecalcPopcnt(), S2.getPrecalcPopcnt(), taxa);
}
SpiAlgo::SpiAlgo(size_t split_len) : GeneralizedRfAlgo(split_len) {
}

RfAlgorithmInterface::Scalar SpiAlgo::calc_split_score(const PllSplit &S1) {
	return S1.getHInfoContent();
}

// AB | DEFC
// DEF | ABC --> ABC | DEF

// ABC | DE
// ADE | BC