//
// Created by Robert Schmoltzi on 18.05.21.
//

#include "MciAlgo.h"

// TODO: test boolean transformation to reduce operations
double
MciAlgo::calc_split_score( PllSplit &S1,  PllSplit &S2, size_t taxa, size_t split_len) {
	 auto a1 = S1.pre_calc_popcount;
	 auto a2 = S2.pre_calc_popcount;
	 auto b1 = taxa - a1;
	 auto b2 = taxa - a2;

	compute_split_comparison(S1, S2, split_len);

	 auto a1_a2 = temporary_splits[2].popcount(split_len); // A1_and_A2
	 auto a1_b2 = temporary_splits[4].popcount(split_len); // A1_and_B2
	 auto a2_b1 = temporary_splits[5].popcount(split_len); // B1_and_A2

	// Account for the bits counted at the end because of both inversions!
	// TODO: test whether bitmask is faster
	 auto bits_too_many = GeneralizedRfAlgo::bits_too_many(taxa);
	 auto b1_b2 =
	    temporary_splits[3].popcount(split_len) - bits_too_many; // B1_and_B2
	assert(b1_b2 == taxa - (a1_a2 + a1_b2 + a2_b1));

	assert(a1 == a1_a2 + a1_b2); // na
	assert(a2 == a1_a2 + a2_b1); // nb
	assert(b1 == a2_b1 + b1_b2); // nA
	assert(b2 == a1_b2 + b1_b2); // nB
	 auto sum_a1_a2 = to_prob(a1_a2, taxa, a1, a2);
	 auto sum_a1_b2 = to_prob(a1_b2, taxa, a1, b2);
	 auto sum_a2_b1 = to_prob(a2_b1, taxa, a2, b1);
	 auto sum_b1_b2 = to_prob(b1_b2, taxa, b1, b2);

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
	 size_t numerator = numerator_inout_log * numerator_in_log;
	 size_t denominator = denom_a * denom_b;

	// log first should result in a higher precision
	return static_cast<Scalar>(numerator_inout_log) *
	       (factorials.lg(numerator) - factorials.lg(denominator));
}

RfAlgorithmInterface::Scalar
MciAlgo::calc_tree_info_content( PllSplitList &S, size_t taxa, size_t split_len) {
	GeneralizedRfAlgo::Scalar sum = 0;
	for (size_t i = 0; i < S.size(); ++i) {
		// TODO: Perf: log2(a) - log2(x);
		auto a = static_cast<Scalar>(S[i].popcount(split_len));
		auto b = static_cast<Scalar>(taxa) - static_cast<Scalar>(a);
		auto taxa_f = static_cast<Scalar>(taxa);
		auto entropy = -a / taxa_f * std::log2(a / taxa_f) - b / taxa_f * std::log2(b / taxa_f);
		sum += entropy;
	}
	return sum;
}
GeneralizedRfAlgo::SplitScores MciAlgo::calc_pairwise_split_scores(PllSplitList &S1,
                                                                   PllSplitList &S2) {
    SplitScores scores(S1.size());
    auto taxa = S1.size() + 3;
    factorials.reserve(taxa + taxa);
    auto split_len = S1.computeSplitLen();
    for (size_t row = 0; row < S1.size(); ++row) {
		S1[row].set_not(split_len, &temporary_split_content[0]);
        for (size_t col = 0; col < S1.size(); ++col) {
            S2[col].set_not(split_len, &temporary_split_content[split_len]);
            auto val = calc_split_score(S1[row], S2[col], taxa, split_len);
            if (scores.max_score < val) {
                scores.max_score = val;
            }
            scores.scores.set(row, col, val);
        }
    }

    return scores;
}

void MciAlgo::compute_split_comparison(PllSplit &S1,
                                                 PllSplit &S2,
                                                 size_t split_len) {
    // A1 and A2 -> &split_buffer[2 * split_len]
    S1.intersect(S2, split_len, &temporary_split_content[2 * split_len]);
    // B1 and B2 -> &split_buffer[3 * split_len]
    temporary_splits[0].intersect(
        temporary_splits[1], split_len, &temporary_split_content[3 * split_len]);
    // A1 and B2 -> &split_buffer[4 * split_len]
    S1.intersect(temporary_splits[1], split_len, &temporary_split_content[4 * split_len]);
    // A2 and B1 -> &split_buffer[5 * split_len]
    S2.intersect(temporary_splits[0], split_len, &temporary_split_content[5 * split_len]);
}
