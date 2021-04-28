//
// Created by Robert Schmoltzi on 27.04.21.
//
#include "PllSplits.hpp"
#include <catch2/catch.hpp>
#include <PllTree.hpp>
#include <iostream>
#include "../helpers/Util.h"

/* Reference implementation for the pop count. */
static size_t ref_popcount(const PllSplit& split, size_t len) {
    size_t popcount = 0;
    for (size_t index = 0; index < len * PllSplit::splitBitWidth(); ++index) {
        if (split.bitExtract(index) == 1) { popcount += 1; }
    }

    return popcount;
}

TEST_CASE("popcount", "[primitives]") {
    auto tree_single_str = Util::getOutputOfBashCommand("head -n 20 /rf_data/BS/125");
    auto tree_str = Util::split(tree_single_str, '\n');
    std::vector<PllSplit> splits;

    unsigned int tip_count = PllTree(tree_str[0]).tree()->tip_count;
    unsigned int tip_ints = (tip_count + 31) / 32;

    std::cout << "tip_count: " << tip_count << std::endl << "tip_ints: " <<  tip_ints << std::endl;

    for (const auto& s : tree_str) {
        PllSplitList tmp = PllTree(s);
        for (size_t i = 0; i < tmp.size(); i++) {
            splits.emplace_back(tmp[i]);
        }
    }

    for (auto spl : splits) {
        // std::cout << spl.popcount(tip_ints) << std::endl;
        CHECK(spl.popcount(tip_ints) == ref_popcount(spl, tip_ints));
    }
}


