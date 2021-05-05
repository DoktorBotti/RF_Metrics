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

// I LIED, its only the first tree!
static std::string get_first_n_trees(size_t N = 1) {
	assert(N == 1);
	std::string temporaryPath = "/tmp/tmpoutput.txt";
	std::string command = "head -n 1 /rf_data/BS/24 >" + temporaryPath;
	// command = "ls /";
	system(command.c_str());
	std::stringstream ss;
	auto stream = std::ifstream(temporaryPath);
	ss << stream.rdbuf();
	CHECK(stream.is_open());
	std::string out = ss.str();
	return out;
}

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

	for (auto & t : tree_list) {
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
	std::string firstTree = get_first_n_trees(1);
	std::vector<PllTree> tree_list;
	tree_list.emplace_back(firstTree);

	std::vector<PllSplitList> splits_list;

	for (auto & t : tree_list) {
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

TEST_CASE("Calc standard RF distance", "[RF]") {
	RfMetricInterface::Params libParams = {
	    1, "/rf_data/BS/125", "/tmp/foobaz.out", RfMetricInterface::Metric::RF};
	RfMetricInterface algo(libParams);
	algo.do_magical_high_performance_stuff();
	INFO(algo.get_result().num_unique_trees);
	INFO(algo.get_result().mean_distance);
	INFO(algo.get_result().pairwise_distances.checked_at(5, 20));
	REQUIRE(false);
}