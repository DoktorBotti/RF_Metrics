//
// Created by Robert Schmoltzi on 19.05.21.
//

#include "SpiAlgo.h"
#include <boost/math/special_functions/factorials.hpp>
double SpiAlgo::calc_tree_score(const PllSplitList &S1, const PllSplitList &S2) {
	return 0;
}

double
SpiAlgo::calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) {
	if (!compatible(S1, S2, split_len)) {
		return 0;
	}
	return h_info_content(S1, taxa, split_len) + h_info_content(S2, taxa, split_len) -
	       h_info_content(S1, S2, taxa, split_len);
}

bool SpiAlgo::compatible(const PllSplit &S1, const PllSplit &S2, size_t split_len) {
	// TODO: Maybe vectorize
	std::vector<pll_split_base_t> inv_bitset_buffer(split_len * 2);
	S1.set_not(split_len, &inv_bitset_buffer[0]);
	S2.set_not(split_len, &inv_bitset_buffer[split_len]);
	return S1.is_disjoint(S2, split_len) ||
	       S1.is_disjoint(PllSplit(&inv_bitset_buffer[split_len]), split_len) ||
	       S2.is_disjoint(PllSplit(&inv_bitset_buffer[0]), split_len);
}
