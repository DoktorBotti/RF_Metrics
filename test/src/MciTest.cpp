//
// Created by Robert Schmoltzi on 18.05.21.
//

#include "MciAlgo.h"
#include <catch2/catch.hpp>
#include <rf/helpers/Util.h>

TEST_CASE("Test info_cl", "[MCI][GenRF]") {
	auto split_lists = Util::create_splits(2, "/rf_data/BS/125");
    auto res = MciAlgo::info_cl(split_lists[0][0], split_lists[1][0], split_lists[0].size(), split_lists[0].computeSplitLen());
    INFO(res);
	REQUIRE(false);
}