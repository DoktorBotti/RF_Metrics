//
// Created by Robert Schmoltzi on 18.05.21.
//

#include "MciAlgo.h"

SymmetricMatrix<double> MciAlgo::calc_pairwise_split_scores(const PllSplitList &S1,
                                                            const PllSplitList &S2) {
	SymmetricMatrix<double> scores(S1.size());
	auto taxa = S1.size() + 3;
	for (size_t row = 0; row < S1.size(); ++row) {
		for (size_t col = 0; col <= row; ++col) {
			// TODO: constexpr computeSplitLen or use attribute
			scores.set_at(row, col, info_cl(S1[row], S2[col], taxa, S1.computeSplitLen()));
		}
	}

	return scores;
}

double MciAlgo::info_cl(PllSplit split1, PllSplit split2, size_t taxa, size_t split_len) {
	double inv_pcl_a1 = (double) taxa / (double) split1.popcount(split_len);
	double inv_pcl_a2 = (double) taxa / (double) split2.popcount(split_len);
	double inv_pcl_b1 = (double) taxa / (double) (taxa - split1.popcount(split_len));
	double inv_pcl_b2 = (double) taxa / (double) (taxa - split2.popcount(split_len));

	// TODO: is there something more lightweight (like an array?)
	std::vector<pll_split_base_t> tmp(split_len);
	split1.intersect(split2, split_len, &tmp[0]);
	double pcl_a1_a2 = (double) PllSplit(&tmp[0]).popcount(split_len) / (double) taxa;

	std::vector<pll_split_base_t> tmp1(split_len * 2);
	split2.set_not(split_len, &tmp1[split_len]);
	split1.intersect(PllSplit(&tmp1[split_len]), split_len, &tmp1[0]);
	double pcl_a1_b2 = (double) PllSplit(&tmp1[0]).popcount(split_len) / (double) taxa;

	std::vector<pll_split_base_t> tmp2(split_len * 2);
	split1.set_not(split_len, &tmp2[split_len]);
	split2.intersect(PllSplit(&tmp2[split_len]), split_len, &tmp2[0]);
	double pcl_a2_b1 = (double) PllSplit(&tmp2[0]).popcount(split_len) / (double) taxa;

	std::vector<pll_split_base_t> tmp3(split_len * 3);
	split1.set_not(split_len, &tmp3[split_len]);
	split2.set_not(split_len, &tmp3[split_len * 2]);
	PllSplit(&tmp3[split_len * 2]).intersect(PllSplit(&tmp3[split_len]), split_len, &tmp3[0]);
	// Account for the bits counted because of both inversions!!!!!
	constexpr size_t bit_amount_split = sizeof(pll_split_base_t) * 8;
	auto bits_too_much =
	    taxa % bit_amount_split == 0 ? 0 : bit_amount_split - (taxa % bit_amount_split);
	double pcl_a2_b2 =
	    (double) (PllSplit(&tmp3[0]).popcount(split_len) - bits_too_much) / (double) taxa;

	return pcl_a1_a2 * (std::log(pcl_a1_a2 * inv_pcl_a1 * inv_pcl_a2)) +
	       pcl_a1_b2 * (std::log(pcl_a1_b2 * inv_pcl_a1 * inv_pcl_b2)) +
	       pcl_a2_b1 * (std::log(pcl_a2_b1 * inv_pcl_a2 * inv_pcl_b1)) +
	       pcl_a2_b2 * (std::log(pcl_a2_b2 * inv_pcl_a2 * inv_pcl_b2));
}
