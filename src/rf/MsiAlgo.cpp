#include "MsiAlgo.h"

MsiAlgo::MsiAlgo(size_t split_len) : GeneralizedRfAlgo(split_len) {
}

RfAlgorithmInterface::Scalar
MsiAlgo::calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) {
	// TODO: could save one interim result with boolean transformation -> test performance
	// TODO: are there ways to predict which calculation returns the greater value?
    compute_split_comparison(S1, S2, split_len);

    const auto bits_too_many = GeneralizedRfAlgo::bits_too_many(taxa);
    const auto a1_a2 = temporary_splits[2].popcount(split_len);
    const auto b1_b2 = temporary_splits[3].popcount(split_len) - bits_too_many;
    const auto a1_b2 = temporary_splits[4].popcount(split_len);
    const auto a2_b1 = temporary_splits[5].popcount(split_len);

	// trivial splits contain no information
	if (std::min(a1_a2, b1_b2) <= 1 && std::min(a1_b2, a2_b1) <= 1) { // Try a1_a2 <= 1 && b1_b2 <=
		                                                              // 1 && ...
		// all splits are trivial
		return 0;
	} else if (std::min(a1_a2, b1_b2) <= 1) {
		// only second split is non-trivial
		return h_info_content(a1_b2, a2_b1);
	} else if (std::min(a1_b2, a2_b1) <= 1) {
		// only first split is non-trivial
		return h_info_content(a1_a2, b1_b2);
	}
	return std::max(h_info_content(a1_a2, b1_b2), h_info_content(a1_b2, a2_b1));
}
RfAlgorithmInterface::Scalar MsiAlgo::calc_split_score(const PllSplit &S1, size_t taxa) {
	return S1.getHInfoContent();
}
