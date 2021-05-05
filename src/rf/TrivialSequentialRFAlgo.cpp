//
// Created by Robert Schmoltzi on 03.05.21.
//

#include "include/TrivialSequentialRFAlgo.h"

RfMetricInterface::Results TrivialSequentialRFAlgo::calculate(std::vector<PllTree> &trees) {
	RfMetricInterface::Results result(trees.size());

	for (auto &t : trees) {
		t.alignNodeIndices(*trees.begin());
	}

	for (size_t row = 0; row < trees.size(); ++row) {
		for (size_t col = 0; col < row; ++col) {
			result.pairwise_distances_absolute.set_at(row, col, rf_dist(trees[row], trees[col]));
		}
	}

	return result;
}

size_t TrivialSequentialRFAlgo::rf_dist(const PllTree &t0, const PllTree &t1) {
	auto splits0 = PllSplitList(t0);
	auto splits1 = PllSplitList(t1);
	auto len = splits0.computeSplitLen();
	size_t dist = 2 * splits1.size(); // Maximum rf distance

	for (size_t i = 0; i < splits1.size(); ++i) {
		if (splits_contain(splits0, splits1[i], len)) {
			dist -= 2;
		}
	}

	return dist;
}

bool inline TrivialSequentialRFAlgo::splits_contain(const PllSplitList &splits,
                                                    const PllSplit &split,
                                                    size_t len) {
	for (size_t i = 0; i < splits.size(); ++i) {
		if (splits[i].equals(split, len)) {
			return false;
		}
	}
	return true;
}