#include "../helpers/Util.h"
#include "PllSplits.hpp"
#include "PllTree.hpp"
#include "RfMetricInterface.h"
#include "GeneralizedRfAlgo.h"
#include <catch2/catch.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

TEST_CASE("read first test tree", "[data]") {
	std::string temporaryPath = "/home/user/tmpoutput.txt";
	std::string command = "head -n 1 /rf_data/BS/24 >" + temporaryPath;
	// command = "ls /";
	std::system(command.c_str());
	std::stringstream ss;
	auto stream = std::ifstream(temporaryPath);
	ss << stream.rdbuf();
	CHECK(stream.is_open());
	std::string out = ss.str();
	REQUIRE(out.length() > 0);
}

TEST_CASE("Double fact. test", "[factorial]"){
	auto& fac = GeneralizedRfAlgo::factorials;

	CHECK(std::abs(fac.lg(2) - 1) <= 1e-8);
	CHECK(std::abs(fac.lg_dbl_fact(-1) - 0) <= 1e-8);
	CHECK(std::abs(fac.lg_dbl_fact(15) - 20.950932) < 1e-4);
	CHECK(std::abs(fac.lg_rooted_dbl_fact(7) - 13.34360213) < 1e-4);
	CHECK(std::abs(fac.lg_unrooted_dbl_fact(7) - 9.884170519) < 1e-4);
}
TEST_CASE("Print first Split", "[useless]") {
	auto tree_list = Util::create_trees(1, "/rf_metrics/BS/24");

	std::vector<PllSplitList> splits_list;

	for (auto &t : tree_list) {
		t.alignNodeIndices(*tree_list.begin());
		splits_list.emplace_back(t);
	}
	pllmod_utree_split_show(splits_list[0][0](), 5);
}

TEST_CASE("Check CPU output", "[useless]") {
	const std::string command = "lscpu";
	INFO(Util::get_output_of_bash_command(command));
	REQUIRE(false);
}

TEST_CASE("Checked methods SymmetricMatrix", "[SymMtx]") {
	SymmetricMatrix<size_t> mat(2);
	const size_t val = 1337;
	mat.checked_set_at(0,1,val);
	REQUIRE(mat.checked_at(0,1) == val);
	REQUIRE(mat.at(1,0) == val);
}