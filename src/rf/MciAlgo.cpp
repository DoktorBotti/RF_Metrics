//
// Created by Robert Schmoltzi on 18.05.21.
//

#include "MciAlgo.h"

// TODO: test boolean transformation to reduce operations
double
MciAlgo::calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) {
	const auto a1 = S1.popcount(split_len);
	const auto a2 = S2.popcount(split_len);
	const auto b1 = taxa - a1;
	const auto b2 = taxa - a2;

	auto split_buffer = GeneralizedRfAlgo::compute_split_comparison(S1, S2, split_len);

	const auto a1_a2 = PllSplit(&split_buffer[2 * split_len]).popcount(split_len); // a_and_b
	const auto a1_b2 = PllSplit(&split_buffer[4 * split_len]).popcount(split_len); // a_and_B
	const auto a2_b1 = PllSplit(&split_buffer[5 * split_len]).popcount(split_len); // A_and_b
	// Account for the bits counted at the end because of both inversions!
	// TODO: test whether bitmask is faster
	const auto bits_too_many = GeneralizedRfAlgo::bits_too_many(taxa);
	const auto b1_b2 =
	    PllSplit(&split_buffer[3 * split_len]).popcount(split_len) - bits_too_many; // A_and_B
	assert(b1_b2 == taxa - (a1_a2 + a1_b2 + a2_b1));

	assert(a1 == a1_a2 + a1_b2); // na
	assert(a2 == a1_a2 + a2_b1); // nb
	assert(b1 == a2_b1 + b1_b2); // nA
	assert(b2 == a1_b2 + b1_b2); // nB
	const auto sum_a1_a2 = to_prob(a1_a2, taxa, a1, a2);
	const auto sum_a1_b2 = to_prob(a1_b2, taxa, a1, b2);
	const auto sum_a2_b1 = to_prob(a2_b1, taxa, a2, b1);
	const auto sum_b1_b2 = to_prob(b1_b2, taxa, b1, b2);

	return (sum_a1_a2 + sum_a1_b2 + sum_a2_b1 + sum_b1_b2) / static_cast<Scalar>(taxa);
}
RfAlgorithmInterface::Scalar MciAlgo::to_prob(size_t numerator_inout_log,
                                              size_t numerator_in_log,
                                              size_t denom_a,
                                              size_t denom_b) {
	if (!numerator_inout_log || !denom_b || !denom_a) {
		// return 0 if outer prod is zero or denominator is zeros
		return 0;
	}
	// check case when log2(2) = 1
	if (numerator_inout_log == denom_a && numerator_inout_log == denom_b &&
	    numerator_inout_log + numerator_inout_log == numerator_in_log) {
		return static_cast<Scalar>(numerator_inout_log);
	}
	const size_t numerator = numerator_inout_log * numerator_in_log;
	const size_t denominator = denom_a * denom_b;

	// log first should result in a higher precision
	return static_cast<Scalar>(numerator_inout_log) *
	       (factorials.lg(numerator) - factorials.lg(denominator));
}

RfAlgorithmInterface::Scalar
calc_tree_info_content(const PllSplitList &S, size_t taxa, size_t split_len) {
	GeneralizedRfAlgo::Scalar sum = 0;
    for (size_t i = 0; i < S.size(); ++i) {
		auto a = S[i].popcount(split_len);
		auto pcl_a = static_cast<GeneralizedRfAlgo::Scalar>(a) / static_cast<GeneralizedRfAlgo::Scalar>(taxa);
		auto lg_pcl_b = std::log((1-pcl_a));
        sum += -pcl_a * std::log2(pcl_a) - lg_pcl_b + pcl_a * lg_pcl_b; // TODO: Perf: log2(a) - log2(x);
    }
    return sum;
}
