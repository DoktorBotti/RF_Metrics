//
// Created by Robert Schmoltzi on 03.05.21.
//

#include <TrivialSequentialRFAlgo.h>
#include <catch2/catch.hpp>
#include <filesystem>
#include <iostream>
#include <utility>
#include <rf/helpers/Util.h>

TEST_CASE("trivial sequential RF algorithm", "[rf-algo]") {
	auto trees = Util::create_trees(9, "/rf_data/BS/125");
	TrivialSequentialRFAlgo alg;
	auto res = alg.calculate(trees);
	// TODO something to actually compare against
}

TEST_CASE("Calc standard RF distance", "[RF]") {
	RfMetricInterface::Params libParams = {
	    1, "/rf_data/BS/125", "/tmp/foobaz.out", RfMetricInterface::Metric::RF};
	RfMetricInterface algo(libParams);
	algo.do_magical_high_performance_stuff();
	INFO(algo.get_result().num_unique_trees);
	INFO(algo.get_result().mean_distance);
	REQUIRE(false);
}

static io::IOData calc_result(const std::string& in_file);
static io::IOData get_raxml_res(const std::string &base_path_res, const std::string &test_fname);

TEST_CASE("RFdst: Compare all samples", "[RF]")
{
	std::string base_path = "../test/samples/data/heads/BS/";
	std::string base_path_res = "../test/samples/reference_results/";
	// For all files in directory
	for (const auto &entry : std::filesystem::directory_iterator(base_path)) {
		// skip irrelevant files
		if (entry.path().filename() == "." || entry.path().filename() == "..") {
			continue;
		}
		CHECK(entry.is_regular_file());

		std::string test_fname = entry.path().filename();
        io::IOData results = calc_result(entry.path().string());
		io::IOData raxml_res = get_raxml_res(base_path_res, test_fname);

		// compare results
		CHECK(raxml_res == results);

	}
}
static io::IOData get_raxml_res(const std::string &base_path_res,
                                const std::string &test_fname) { // load RAxML
	std::string info_path, pairwise_dst_path;
	bool found_info = false;
	bool found_pairwise = false;

	// find information in files
	for (const auto &resultEntry :
         std::filesystem::directory_iterator(base_path_res + test_fname)) {
        if (resultEntry.path().string().find("info") != std::string::npos) {
            info_path = resultEntry.path().string();
            found_info = true;
        } else if (resultEntry.path().string().find("RF-Distances") != std::string::npos) {
            found_pairwise = true;
            pairwise_dst_path = resultEntry.path().string();
        }
        if(found_info && found_pairwise){
            break;
		}
	}
	// parse RAxML
	    io::IOData raxml_res;
	    io::IOData::parse_raxml(info_path, pairwise_dst_path, raxml_res);
	    return raxml_res;
}
static io::IOData calc_result(const std::string& in_file) {
	RfMetricInterface::Params params;
	params.input_file_path = in_file;
	params.metric = RfMetricInterface::RF;
	params.threads = 1;
	RfMetricInterface iface(params);
	iface.do_magical_high_performance_stuff();
	return iface.get_result_as_IOData();
}
