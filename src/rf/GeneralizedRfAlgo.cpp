//
// Created by Tobia on 16.05.2021.
//

#include "include/GeneralizedRfAlgo.h"
#include <boost/math/special_functions/factorials.hpp>

GeneralizedRfAlgo::GeneralizedRfAlgo() {
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("generalized_RF"));
}
RfMetricInterface::Results GeneralizedRfAlgo::calculate(std::vector<PllTree> &trees) {
	assert(trees.size() >= 2);
	// extract splits. Each tree now identifies by its index in all_splits
	std::vector<PllSplitList> all_splits;
	all_splits.reserve(trees.size());
	for (auto &t : trees) {
		t.alignNodeIndices(*trees.begin());
		all_splits.emplace_back(t);
	}
	RfMetricInterface::Results res(trees.size());
	double total_dst = 0;

	// iterate through all tree combinations
	for (auto a = std::as_const(all_splits).begin(); a != all_splits.end(); ++a) {
		size_t idx_a = a - all_splits.begin();
		for (auto b = a; b != all_splits.end(); ++b) {
			size_t idx_b = b - a;
			double dst = calc_tree_score(*a, *b);
			res.pairwise_distances_relative.set_at(idx_b, idx_a, dst);
			total_dst += dst;
		}
	}
	// calc mean distance between trees
	res.mean_distance = total_dst / static_cast<double>(trees.size());
	return res;
}
double GeneralizedRfAlgo::phylogenetic_prob(const PllSplit &split_a,
                                                  const PllSplit &split_b) {
	using boost::math::double_factorial;
    const size_t split_len = PllSplit::split_len;
	const size_t pop_1 = split_a.popcount(split_len);
    const size_t pop_2 = split_b.popcount(split_len);
    // no trivial splits here
    assert(pop_1 > 2);
	assert(pop_1 < split_len - 1);
	assert(pop_2 > 2);
	assert(pop_2 < split_len - 1);
	// calc count
	const auto b1_fac = double_factorial<double>(2* (split_len- pop_1) - 3);
	const auto a1_fac = double_factorial<double>(2* pop_1 - 3);
	const auto comb_fac = double_factorial<double>(2* pop_1 - 2*pop_2 - 1);
	const auto dividend = double_factorial<double>(2* split_len - 5);
	//TODO: watch out for numerical issues
	return b1_fac * a1_fac * comb_fac / dividend;
}
double GeneralizedRfAlgo::phylogenetic_prob(const PllSplit &split) {
    using boost::math::double_factorial;
    const size_t split_len = PllSplit::split_len;
    const size_t pop_1 = split.popcount(split_len);
    // no trivial splits here
    assert(pop_1 > 2);
    assert(pop_1 < split_len - 1);

    const auto b1_fac = double_factorial<double>(2* (split_len- pop_1) - 3);
    const auto a1_fac = double_factorial<double>(2* pop_1 - 3);
	const auto dividend = double_factorial<double>(2* split_len - 5);
    return b1_fac * a1_fac  / dividend;

}
