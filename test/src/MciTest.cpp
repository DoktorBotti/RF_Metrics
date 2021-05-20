#include "MciAlgo.h"
#include <catch2/catch.hpp>
#include <rf/helpers/Util.h>
#include <gmpxx.h>

TEST_CASE("Test info_cl", "[MCI][GenRF]") {
	// auto split_lists = Util::create_splits(2, "/rf_data/BS/125");
    // auto res = MciAlgo::calc_split_score(split_lists[0][0], split_lists[1][0], split_lists[0].size(), split_lists[0].computeSplitLen());
    // INFO(res);
	REQUIRE(false);
}

TEST_CASE("Test precision type" , "[GMP]"){
	// manual: https://gmplib.org/manual/C_002b_002b-Class-Interface#index-C_002b_002b-interface
    mpz_class a, b;
    a = 456;
	b = 8999;
	std::stringstream ss;
	ss << "Sum is " << a + b;
	INFO(ss.str());
	REQUIRE(false);
}