//
// Created by Robert Schmoltzi on 18.05.21.
//

#include "MciAlgo.h"

double MciAlgo::calc_split_score(const PllSplit &split1, const PllSplit &split2, size_t taxa, size_t split_len) {
	double inv_pcl_a1 = (double) taxa / (double) split1.popcount(split_len);
	double inv_pcl_a2 = (double) taxa / (double) split2.popcount(split_len);
	double inv_pcl_b1 = (double) taxa / (double) (taxa - split1.popcount(split_len));
	double inv_pcl_b2 = (double) taxa / (double) (taxa - split2.popcount(split_len));

	// TODO: is there something more lightweight (like an array?)
	std::vector<pll_split_base_t> tmp(split_len);
	split1.intersect(split2, split_len, &tmp[0]);
	double pcl_a1_a2 = (double) PllSplit(&tmp[0]).popcount(split_len) / (double) taxa;
	assert(std::isfinite(pcl_a1_a2));

	std::vector<pll_split_base_t> tmp1(split_len * 2);
	split2.set_not(split_len, &tmp1[split_len]);
	split1.intersect(PllSplit(&tmp1[split_len]), split_len, &tmp1[0]);
	double pcl_a1_b2 = (double) PllSplit(&tmp1[0]).popcount(split_len) / (double) taxa;
	assert(std::isfinite(pcl_a1_b2));

	std::vector<pll_split_base_t> tmp2(split_len * 2);
	split1.set_not(split_len, &tmp2[split_len]);
	split2.intersect(PllSplit(&tmp2[split_len]), split_len, &tmp2[0]);
	double pcl_a2_b1 = (double) PllSplit(&tmp2[0]).popcount(split_len) / (double) taxa;
    assert(std::isfinite(pcl_a2_b1));

	std::vector<pll_split_base_t> tmp3(split_len * 3);
	split1.set_not(split_len, &tmp3[split_len]);
	split2.set_not(split_len, &tmp3[split_len * 2]);
	PllSplit(&tmp3[split_len * 2]).intersect(PllSplit(&tmp3[split_len]), split_len, &tmp3[0]);
	// Account for the bits counted because of both inversions!!!!!
	const auto bits_too_much = GeneralizedRfAlgo::bits_too_many(taxa);
	double pcl_a2_b2 =
	    (double) (PllSplit(&tmp3[0]).popcount(split_len) - bits_too_much) / (double) taxa;
	assert(std::isfinite(pcl_a2_b2));

	return pcl_a1_a2 * (std::log(pcl_a1_a2 * inv_pcl_a1 * inv_pcl_a2)) +
	       pcl_a1_b2 * (std::log(pcl_a1_b2 * inv_pcl_a1 * inv_pcl_b2)) +
	       pcl_a2_b1 * (std::log(pcl_a2_b1 * inv_pcl_a2 * inv_pcl_b1)) +
	       pcl_a2_b2 * (std::log(pcl_a2_b2 * inv_pcl_a2 * inv_pcl_b2));
}
