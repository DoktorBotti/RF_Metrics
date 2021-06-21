#include "../src/rf/helpers/Util.h"
#include "RfMetricInterface.h"
#include "catch2/catch.hpp"
#include <boost/log/sources/record_ostream.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

static void test_metric(const std::string &base_path_splits,
                        const std::string &base_path_res,
                        const RfMetricInterface::Metric &metric);
TEST_CASE("SPI compare un-normalized", "[SPI][un-normalized][long]") {
	// This test needs the pre-calculated results in /rf_metrics/SPI_10/
	// misc/createTestingData.R generates this content

	std::string base_path = "../test/samples/data/heads/BS/";
	std::string base_path_res = "/rf_metrics/SPI_10/";
	auto metr = RfMetricInterface::SPI;
	test_metric(base_path, base_path_res, metr);
}
TEST_CASE("MSI compare un-normalized", "[MSI][un-normalized][long]") {
	// This test needs the pre-calculated results in /rf_metrics/MSI_10/
	// misc/createTestingData.R generates this content

	std::string base_path = "../test/samples/data/heads/BS/";
	std::string base_path_res = "/rf_metrics/MSI_10/";
	auto metr = RfMetricInterface::MSI;
	test_metric(base_path, base_path_res, metr);
}

TEST_CASE("MCI compare un-normalized", "[MCI][un-normalized][long]") {
	// This test needs the pre-calculated results in /rf_metrics/SPI_10/
	// misc/createTestingData.R generates this content

	std::string base_path = "../test/samples/data/heads/BS/";
	std::string base_path_res = "/rf_metrics/MCI_10/";
	auto metr = RfMetricInterface::MCI;
	test_metric(base_path, base_path_res, metr);
}
static void write_to_file(const std::string &file, const std::string &content);
static std::string
combine_trees_to_file(const std::string &source_tree_file, size_t tree_idx_a, size_t tree_idx_b);
static bool nearly_eq_floating(double a, double b);

TEST_CASE("Calculate simple trees", "[dbg]") {
	// Edit your trees to test here
	std::string tree1 = "((((F,C),B),A),(E,D));";
	std::string tree2 = "(((B,C),(D,A)),(E,F));";
	std::string tree_path = "/tmp/tmpTrees";

	// write them to a temporary file
	auto str = tree1 + "\n" + tree2;
	write_to_file(tree_path, str);

	// init algo interface
	RfMetricInterface::Params params;
	params.normalize_output = false;
	params.metric = RfMetricInterface::SPI;
	params.input_file_path = tree_path;

	RfMetricInterface iface(params);
	iface.do_magical_high_performance_stuff();
	std::cout << iface.get_result().pairwise_similarities.print() << std::endl << std::flush;
	SUCCEED("Done.");
}

TEST_CASE("Compare to other Team", "[other_team]") {
	std::string base_path_res = "../test/samples/luise_reference/";
	std::string base_path = "../test/samples/data/heads/BS/";

	for (const auto &entry : std::filesystem::directory_iterator(base_path_res)) {
		// skip irrelevant files
		if (entry.path().filename() == "." || entry.path().filename() == "..") {
			continue;
		}
		CHECK_FALSE(entry.is_directory());
		// parse which parameters to pass
		std::string res_fname = entry.path().filename();
		auto splitted_name = Util::split(res_fname, '_');
		auto metr = RfMetricInterface::MCI;
		{
			if (splitted_name[0] == "MCI") {
				metr = RfMetricInterface::MCI;
			} else if (splitted_name[0] == "MSI") {
				metr = RfMetricInterface::MSI;
			} else if (splitted_name[0] == "SPI") {
				metr = RfMetricInterface::SPI;
			} else {
				INFO("Failed to parse metric from other teams reference")
				INFO(splitted_name[0])
				REQUIRE(false);
			}
		}
		std::string test_name = Util::split(splitted_name[1], '.')[0];

		// load other team IOData
		auto res_string = Util::read_file(entry.path().string());
		nlohmann::json res_json = nlohmann::json::parse(res_string);
		io::IOData res;
		io::from_json(res_json, res);
		// our execution
		RfMetricInterface::Params params;
		params.input_file_path = base_path + test_name;
		params.metric = metr;
		RfMetricInterface iface(params);
		iface.do_magical_high_performance_stuff();
		auto our_res = iface.get_result_as_IOData();
		// compare results
		CHECK(our_res.comparePairwiseDistances(res));

		// report maximal total difference
		double max_diff = 0.0;
		double mean_diff = 0.;
		size_t el_counter = 0;
		size_t n_rows = our_res.pairwise_distance_mtx.size();
		REQUIRE(n_rows == res.pairwise_distance_mtx.size());
		for (size_t row = 0; row < n_rows; ++row) {
			size_t n_cols = our_res.pairwise_distance_mtx[row].size();
			REQUIRE(n_cols == res.pairwise_distance_mtx[row].size());
			for(size_t col = 0; col < n_cols; ++col){
				auto diff = std::abs(our_res.pairwise_distance_mtx[row][col] - res.pairwise_distance_mtx[row][col]);
				if(diff > max_diff){
					max_diff  = diff;
				}
				++el_counter;
				mean_diff += diff;
			}
		}
		mean_diff /= static_cast<double>(el_counter);
		WARN("[" << splitted_name[0] << "] Mean Difference: " << mean_diff << "\n[" << splitted_name[0] << " Max Difference: " << max_diff);
	}
}
static void test_metric(const std::string &base_path_splits,
                        const std::string &base_path_res,
                        const RfMetricInterface::Metric &metric) {
	std::random_device rand_dev;
	std::mt19937 rnd(rand_dev());
	std::uniform_int_distribution<int> tree_idx_distr(0, 9);
	// For each result, run our <metric> algorithm on two (random) contained trees
	for (const auto &entry : std::filesystem::directory_iterator(base_path_res)) {
		// skip irrelevant files
		if (entry.path().filename() == "." || entry.path().filename() == "..") {
			continue;
		}
		CHECK(entry.is_directory());
		std::string res_fname = entry.path().filename();

		// generate problem instance between only two trees from data
		auto tree_idx_a = static_cast<size_t>(tree_idx_distr(rnd));
		auto tree_idx_b = static_cast<size_t>(tree_idx_distr(rnd));
		std::string trees_path =
		    combine_trees_to_file(base_path_splits + res_fname, tree_idx_a, tree_idx_b);

		// Perform calculation on problem instance
		RfMetricInterface::Params params;
		params.input_file_path = trees_path;
		params.metric = metric;
		params.normalize_output = false;
		INFO("Calculating distances from " + res_fname)
		INFO("Indices a: " + std::to_string(tree_idx_a) + "    b: " + std::to_string(tree_idx_b))
		CHECK(!res_fname.empty());
		RfMetricInterface iface(params);
		iface.do_magical_high_performance_stuff();
		auto res = iface.get_result().pairwise_similarities;

		// load solution matrix
		SymmetricMatrix<double> true_mtx =
		    Util::parse_sym_mtx_from_r(base_path_res + res_fname + "/pairwise_trees");
		// compare tree score with it self
		double a_a = true_mtx.checked_at(tree_idx_a, tree_idx_a);
		INFO("Comparing ours to reference: " + std::to_string(a_a) + " <-> " +
		     std::to_string(res.checked_at(0, 0)))
		CHECK(nearly_eq_floating(a_a, res.checked_at(0, 0)));

		double b_b = true_mtx.checked_at(tree_idx_b, tree_idx_b);
		INFO("Comparing ours to reference: " + std::to_string(b_b) + " <-> " +
		     std::to_string(res.checked_at(1, 1)))
		CHECK(nearly_eq_floating(b_b, res.checked_at(1, 1)));
		// compare across trees
		double a_b = true_mtx.checked_at(tree_idx_a, tree_idx_b);
		INFO("Comparing ours to reference: " + std::to_string(a_b) + " <-> " +
		     std::to_string(res.checked_at(1, 0)))
		CHECK(nearly_eq_floating(a_b, res.checked_at(1, 0)));

		WARN(a_b - res.checked_at(1, 0));

	}
}
static std::string
combine_trees_to_file(const std::string &source_tree_file, size_t tree_idx_a, size_t tree_idx_b) {
	// essentially write line idx_a and idx_b from source to a output file
	auto idx_a = tree_idx_a <= tree_idx_b ? tree_idx_a : tree_idx_b;
	auto idx_b = tree_idx_a <= tree_idx_b ? tree_idx_b : tree_idx_a;
	std::string a_str, b_str;

	std::ifstream ifstream(source_tree_file);
	REQUIRE(ifstream.is_open());
	std::string line_str;
	size_t line_nr = 0;
	// skip lines until equal
	for (; line_nr <= idx_a; ++line_nr) {
		std::getline(ifstream, line_str);
	}
	a_str = line_str;
	if (idx_a != idx_b) {
		// skip until b
		for (; line_nr <= idx_b; ++line_nr) {
			std::getline(ifstream, line_str);
		}
		b_str = line_str;
	} else {
		b_str = a_str;
	}
	ifstream.close();
	// write to temporary file
	std::string tmp_file_path = "/tmp/pairw_tree_instance";
	std::ofstream out(tmp_file_path);
	REQUIRE(out.is_open());
	// dont accidentally reorder trees
	if (tree_idx_a > tree_idx_b) {
		out << b_str << "\n" << a_str << "\n";

	} else {
		out << a_str << "\n" << b_str << "\n";
	}
	out.close();
	return tmp_file_path;
}

static void write_to_file(const std::string &file, const std::string &content) {
	std::ofstream ostr(file);
	if (ostr.bad() || !ostr.is_open()) {
		throw std::invalid_argument("Could not write to file " + file);
	}
	ostr << content;
	ostr.close();
}

bool nearly_eq_floating(double a, double b) {
	auto absA = std::abs(a);
	auto absB = std::abs(b);
	auto largest = (absA < absB) ? absB : absA;
	auto smallest = (absA < absB) ? absA : absB;
	return largest - smallest <= largest * static_cast<double>(FLT_EPSILON) * 1e-3;
}