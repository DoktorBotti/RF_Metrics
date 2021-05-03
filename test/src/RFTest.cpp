//
// Created by Robert Schmoltzi on 03.05.21.
//

#include <TrivialSequentialRFAlgo.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <rf/helpers/Util.h>

TEST_CASE("trivial sequential RF algorithm", "[rf-algo]") {
	auto trees = Util::create_trees(9, "/rf_data/BS/125");
	TrivialSequentialRFAlgo alg;
	auto res = alg.calculate(trees);
	// TODO something to actually compare against
}