#include "../helpers/Util.h"
#include "PllSplits.hpp"
#include "PllTree.hpp"
#include "RfMetricInterface.h"
#include <catch2/catch.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

TEST_CASE("sample code", "[useless]") {
	std::vector<std::string> tree_strings{
	    "(a, b, (c, d));",
	    "(a, d, (c, b));",
	};
	std::vector<PllTree> tree_list;
	for (auto t : tree_strings) {
		tree_list.emplace_back(t);
	}

	std::vector<PllSplitList> splits_list;

	for (auto &t : tree_list) {
		t.alignNodeIndices(*tree_list.begin());
		splits_list.emplace_back(t);
	}
}

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

TEST_CASE("Print first Split", "[useless]") {
	auto tree_list = Util::create_trees(1, "/rf_data/BS/24");

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
