//
// Created by Robert Schmoltzi on 23.06.21.
//
// Class to quickly check the runtime impact of changes. Danger of overfitting to specific trees.
// Tests should run through pretty fast. They might not work properly with enabled compiler
// optimizations because results can be unused. Importantly, the trees on which the metric is
// calculated are not randomized.

#include <RfMetricInterface.h>
#include <catch2/catch.hpp>

static void time_metric(const RfMetricInterface::Metric &metric);

TEST_CASE("SPI timing test", "[SPI][Time][long]")
{
    auto metr = RfMetricInterface::SPI;
    time_metric(metr);
}

TEST_CASE("MSI timing test", "[MSI][Time][long]")
{
    auto metr = RfMetricInterface::MSI;
    time_metric(metr);
}

TEST_CASE("MCI timing test", "[MCI][Time][long]")
{
	auto metr = RfMetricInterface::MCI;
	time_metric(metr);
}

static void time_metric(const RfMetricInterface::Metric &metric) {
	std::string base_path = "../test/samples/data/heads/BS/";
	std::vector<std::string> trees = {"125", "141", "350"};

	for (const auto &ts : trees) {
		// Perform calculation on problem instance
		RfMetricInterface::Params params;
		params.input_file_path = base_path + ts;
		params.metric = metric;
		params.normalize_output = false;
		INFO("Calculating distances from " + ts);
		CHECK(!ts.empty());
		RfMetricInterface iface(params);
		iface.do_magical_high_performance_stuff();
		auto res = iface.get_result();

		WARN("Result size: " + std::to_string(res.pairwise_similarities.size()));
		WARN("One value: " + std::to_string(res.pairwise_similarities.checked_at(4, 5)));
		WARN("Mean dist?: " + std::to_string(res.mean_distance));
	}
}