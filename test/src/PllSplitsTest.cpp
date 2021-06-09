//
// Created by Robert Schmoltzi on 27.04.21.
//
#include "PllSplits.hpp"
#include "../helpers/Util.h"
#include <PllTree.hpp>
#include <catch2/catch.hpp>
#include <iostream>

/* Reference implementation for the pop count. */
static size_t ref_popcount(const PllSplit & split, size_t len) {
	size_t popcount = 0;
	for (size_t index = 0; index < len * PllSplit::splitBitWidth(); ++index) {
		if (split.bit_extract(index) == 1) {
			popcount += 1;
		}
	}

	return popcount;
}

TEST_CASE("popcount", "[primitives]") {
	auto split_lists = Util::create_splits(20, "/rf_metrics/BS/125");
	unsigned int tip_count = static_cast<unsigned int>(split_lists[0].size()) + 3;
	unsigned int tip_ints = (tip_count + 31) / 32;

	std::cout << "tip_count: " << tip_count << std::endl << "tip_ints: " << tip_ints << std::endl;

	std::vector<PllSplit> splits;
	for (const auto & s : split_lists) {
		for (size_t i = 0; i < s.size(); i++) {
			splits.emplace_back(s[i]);
		}
	}

	for (auto spl : splits) {
		CHECK(spl.popcount(tip_ints) == ref_popcount(spl, tip_ints));
	}
}
