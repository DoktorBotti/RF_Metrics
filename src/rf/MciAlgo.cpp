#include "MciAlgo.h"

MciAlgo::MciAlgo(size_t split_len) : GeneralizedRfAlgo(split_len) {
}
// TODO: test boolean transformation to reduce operations
double
MciAlgo::calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) {
	const auto a1 = S1.getPrecalcPopcnt();
	const auto a2 = S2.getPrecalcPopcnt();
	const auto b1 = taxa - a1;
	const auto b2 = taxa - a2;

	compute_split_comparison(S1, S2, split_len);
	const auto a1_a2 = temporary_splits[2].popcount(split_len); // A1_and_A2
	const auto a1_b2 = temporary_splits[4].popcount(split_len); // A1_and_B2
	const auto a2_b1 = temporary_splits[5].popcount(split_len); // B1_and_A2

	// Account for the bits counted at the end because of both inversions!
	// TODO: test whether bitmask is faster
	const auto bits_too_many = GeneralizedRfAlgo::bits_too_many(taxa);
	const auto b1_b2 = temporary_splits[3].popcount(split_len) - bits_too_many; // B1_and_B2

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

RfAlgorithmInterface::Scalar MciAlgo::calc_tree_info_content(const SplitList &S) {
	GeneralizedRfAlgo::Scalar sum = 0;
	for (size_t i = 0; i < S.size(); ++i) {
		sum += S[i].getHInfoContent();
	}
	return sum;
}

RfAlgorithmInterface::Scalar MciAlgo::calc_split_score(const PllSplit &S1, size_t taxa) {
	return (to_prob(S1.getPrecalcPopcnt(), taxa, S1.getPrecalcPopcnt(), S1.getPrecalcPopcnt()) +
	        to_prob(taxa - S1.getPrecalcPopcnt(),
	                taxa,
	                taxa - S1.getPrecalcPopcnt(),
	                taxa - S1.getPrecalcPopcnt())) /
	       static_cast<Scalar>(taxa);
}
RfAlgorithmInterface::Scalar MciAlgo::h_info_content(size_t a, size_t b) {
	const auto taxa_f = static_cast<Scalar>(a + b);
	const auto entropy =
	    -static_cast<double>(a) / taxa_f * std::log2(static_cast<double>(a) / taxa_f) -
	    static_cast<double>(b) / taxa_f * std::log2(static_cast<double>(b) / taxa_f);
	return -entropy;
}
