//
// Created by Robert Schmoltzi on 19.05.21.
//

#include "SpiAlgo.h"
#include <boost/math/special_functions/factorials.hpp>

double
SpiAlgo::calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) {
	if (!compatible(S1, S2, taxa, split_len)) {
		return 0;
	}
	GeneralizedRfAlgo::factorials.reserve(taxa + taxa -5);
	//TODO: throw triv splits in da garbage
	return h_info_content(S1, taxa, split_len) + h_info_content(S2, taxa, split_len) -
	       h_info_content(S1, S2, taxa, split_len);
}

bool SpiAlgo::compatible(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) {
	// TODO: Maybe vectorize

	assert(split_len >= 1);

	std::vector<pll_split_base_t> inv_bitset_buffer(split_len * 2);
	S1.set_not(split_len, &inv_bitset_buffer[0]);
	S2.set_not(split_len, &inv_bitset_buffer[split_len]);

	auto bits_too_many = GeneralizedRfAlgo::bits_too_many(taxa);
	pll_split_base_t mask = (~0) << bits_too_many;
	inv_bitset_buffer[split_len - 1] &= mask;

	return S1.is_disjoint(PllSplit(&inv_bitset_buffer[split_len]), split_len) ||  // A1 <-> B2
	       S2.is_disjoint(PllSplit(&inv_bitset_buffer[0]), split_len) ||  // A2 <-> B1
	       PllSplit(&inv_bitset_buffer[0])
	           .is_disjoint(PllSplit(&inv_bitset_buffer[split_len]), split_len);
}

// AB | DEFC
// DEF | ABC --> ABC | DEF


// ABC | DE
// ADE | BC