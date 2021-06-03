#include "catch2/catch.hpp"
#include <filesystem>
#include "RfMetricInterface.h"
#include "../src/rf/helpers/Util.h"

TEST_CASE("SPI compare un-normalized", "[SPI][un-normalized]"){
    // This test needs the pre-calculated results in /rf_metrics/SPI_10/
    // misc/createTestingData.R generates this content

    std::string base_path = "../test/samples/data/heads/BS/";
    std::string base_path_res = "../test/samples/SPI_10";
    // For each result, run our spi algorithm on the first 10 trees
    for (const auto &entry : std::filesystem::directory_iterator(base_path_res)) {
		// skip irrelevant files
		if (entry.path().filename() == "." || entry.path().filename() == "..") {
			continue;
		}
		CHECK(entry.is_regular_file());

		std::string res_fname = entry.path().filename();
        // Perform calculation on problem instance
		RfMetricInterface::Params params;
		params.input_file_path= base_path + res_fname;
		params.metric = RfMetricInterface::SPI;
		params.normalize_output = false;
        RfMetricInterface iface(params);
		iface.do_magical_high_performance_stuff();
		io::IOData res = iface.get_result_as_IOData();

		// load solution matrix
		SymmetricMatrix<double> true_mtx = Util::parse_symmetric_mtx_from_r(base_path_res,'\n', ' ');
		io::IOData true_ioData;
		true_ioData.pairwise_distance_mtx = true_mtx.to_vector();
		CHECK(true_ioData.comparePairwiseDistances(res));
	}
}