//
// Created by Robert Schmoltzi on 18.05.21.
//

#include "MciAlgo.h"

// TODO: test boolean transformation to reduce operations
double
MciAlgo::calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) {
	const auto d_taxa = static_cast<double>(taxa);
	const auto a1 = static_cast<double>(S1.popcount(split_len));
	const auto a2 = static_cast<double>(S2.popcount(split_len));

	const double inv_pcl_a1 = d_taxa / a1;
	const double inv_pcl_a2 = d_taxa / a2;
	// TODO: compare performance to (double) (taxa - S1.popcount(split_len))
	const double inv_pcl_b1 = d_taxa / (d_taxa - a1);
	const double inv_pcl_b2 = d_taxa / (d_taxa - a2);

	auto split_buffer = GeneralizedRfAlgo::compute_split_comparison(S1, S2, split_len);

	const double pcl_a1_a2 =
	    static_cast<double>(PllSplit(&split_buffer[2 * split_len]).popcount(split_len)) / d_taxa;
	assert(std::isfinite(pcl_a1_a2));

	double pcl_a1_b2 =
	    static_cast<double>(PllSplit(&split_buffer[4 * split_len]).popcount(split_len)) / d_taxa;
	assert(std::isfinite(pcl_a1_b2));

	const double pcl_a2_b1 =
	    static_cast<double>(PllSplit(&split_buffer[5 * split_len]).popcount(split_len)) / d_taxa;
	assert(std::isfinite(pcl_a2_b1));

	// Account for the bits counted at the end because of both inversions!
	// TODO: test whether bitmask is faster
	const auto bits_too_many = GeneralizedRfAlgo::bits_too_many(taxa);
	const double pcl_b1_b2 =
	    static_cast<double>(PllSplit(&split_buffer[3 * split_len]).popcount(split_len) -
	                        bits_too_many) /
	    d_taxa;
	assert(std::isfinite(pcl_b1_b2));

	const auto sum_a1_b2 =
	    pcl_a1_b2 == 0 ? 0 : pcl_a1_b2 * (std::log2(pcl_a1_b2 * inv_pcl_a1 * inv_pcl_b2));
	const auto sum_a2_b1 =
	    pcl_a2_b1 == 0 ? 0 : pcl_a2_b1 * (std::log2(pcl_a2_b1 * inv_pcl_a2 * inv_pcl_b1));
	const auto sum_b1_b2 =
	    pcl_b1_b2 == 0 ? 0 : pcl_b1_b2 * (std::log2(pcl_b1_b2 * inv_pcl_a2 * inv_pcl_b2));

    assert(std::isfinite(sum_a1_b2));
    assert(std::isfinite(sum_a2_b1));
    assert(std::isfinite(sum_b1_b2));

	return pcl_a1_a2 * (std::log2(pcl_a1_a2 * inv_pcl_a1 * inv_pcl_a2)) + sum_a1_b2 + sum_a2_b1 +
	       sum_b1_b2;
}
