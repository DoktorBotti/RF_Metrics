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
TEST_CASE("SPI compare un-normalized", "[SPI][un-normalized]") {
	// This test needs the pre-calculated results in /rf_metrics/SPI_10/
	// misc/createTestingData.R generates this content

	std::string base_path = "../test/samples/data/heads/BS/";
	std::string base_path_res = "../test/samples/SPI_10/";
	auto metr = RfMetricInterface::SPI;
	test_metric(base_path, base_path_res, metr);
}
TEST_CASE("MSI compare un-normalized", "[MSI][un-normalized]") {
	// This test needs the pre-calculated results in /rf_metrics/MSI_10/
	// misc/createTestingData.R generates this content

	std::string base_path = "../test/samples/data/heads/BS/";
	std::string base_path_res = "../test/samples/MSI_10/";
	auto metr = RfMetricInterface::MSI;
	test_metric(base_path, base_path_res, metr);
}

TEST_CASE("MCI compare un-normalized", "[MCI][un-normalized]") {
	// This test needs the pre-calculated results in /rf_metrics/SPI_10/
	// misc/createTestingData.R generates this content

	std::string base_path = "../test/samples/data/heads/BS/";
	std::string base_path_res = "../test/samples/MCI_10/";
	auto metr = RfMetricInterface::MCI;
	test_metric(base_path, base_path_res, metr);
}
static void write_to_file(const std::string &file, const std::string &content);
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
	std::cout << iface.get_result().pairwise_distances_relative.print() << std::endl << std::flush;
	SUCCEED("Done.");
}

static void test_metric(const std::string &base_path_splits,
                        const std::string &base_path_res,
                        const RfMetricInterface::Metric &metric) {
	// For each result, run our <metric> algorithm on the first 10 trees
	for (const auto &entry : std::filesystem::directory_iterator(base_path_res)) {
		// skip irrelevant files
		if (entry.path().filename() == "." || entry.path().filename() == "..") {
			continue;
		}
		CHECK(entry.is_directory());

		std::string res_fname = entry.path().filename();
		// Perform calculation on problem instance
		RfMetricInterface::Params params;
		params.input_file_path = base_path_splits + res_fname;
		params.metric = metric;
		params.normalize_output = false;
		RfMetricInterface iface(params);
		iface.do_magical_high_performance_stuff();
		io::IOData res = iface.get_result_as_IOData();

		// load solution matrix
		RectMatrix<double> true_mtx =
		    Util::parse_mtx_from_r(base_path_res + res_fname + "/pairwise_trees");
		io::IOData true_ioData;
		true_ioData.pairwise_distance_mtx = true_mtx.to_vector();
		CHECK(true_ioData.comparePairwiseDistances(res));
	}
}

static void write_to_file(const std::string &file, const std::string &content) {
	std::ofstream ostr(file);
	if (ostr.bad() || !ostr.is_open()) {
		throw std::invalid_argument("Could not write to file " + file);
	}
	ostr << content;
	ostr.close();
}