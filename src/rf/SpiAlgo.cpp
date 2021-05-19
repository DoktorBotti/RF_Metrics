//
// Created by Robert Schmoltzi on 19.05.21.
//

#include "SpiAlgo.h"
#include <boost/math/special_functions/factorials.hpp>
double SpiAlgo::calc_tree_score(const PllSplitList &S1, const PllSplitList &S2) {
	return 0;
}
SymmetricMatrix<double> SpiAlgo::calc_pairwise_split_scores(const PllSplitList &S1,
                                                            const PllSplitList &S2) {

	// TODO: abstract this method into GenRfAlgo, currently duplicate of the Method in Mci Algo

    SymmetricMatrix<double> scores(S1.size());
    const auto taxa = S1.size() + 3;
    const auto split_len = S1.computeSplitLen();
    for (size_t row = 0; row < S1.size(); ++row) {
        for (size_t col = 0; col <= row; ++col) {
            scores.set_at(row, col, calc_split_score(S1[row], S2[col], taxa, split_len));
        }
    }

    return scores;
}

double
SpiAlgo::calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) {
	// TODO: unify naming convention of similar methods
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

double SpiAlgo::h_info_content(const PllSplit &S, size_t taxa, size_t split_len) {
	return (-1) * std::log(p_phy(S, taxa, split_len));
}

double
SpiAlgo::h_info_content(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) {
	return (-1) * std::log(p_phy(S1, S2, taxa, split_len));
}

double inline SpiAlgo::p_phy(const PllSplit &S, size_t taxa, size_t split_len) {
	const auto a = S.popcount(split_len);
	const auto b = taxa - a;

	return boost::math::double_factorial<double>(2 * a - 3) *
	       boost::math::double_factorial<double>(2 * b - 3) /
	       boost::math::double_factorial<double>(2 * (a + b) - 5);
}

double inline SpiAlgo::p_phy(const PllSplit &S1,
                             const PllSplit &S2,
                             size_t taxa,
                             size_t split_len) {
	const auto a1 = S1.popcount(split_len);
	const auto b1 = taxa - a1;
	const auto a2 = S2.popcount(split_len);

	return boost::math::double_factorial<double>(2 * (b1 + 1) - 5) *
	       boost::math::double_factorial<double>(2 * (a2 + 1) - 5) *
	       boost::math::double_factorial<double>(2 * (a1 - a2 + 2) - 5) /
	       boost::math::double_factorial<double>(2 * (taxa) -5);
}
