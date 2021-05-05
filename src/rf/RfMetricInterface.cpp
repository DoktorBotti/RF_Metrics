
#include "RfMetricInterface.h"
#include "gitVersionHeader.h"
#include "helpers/Util.h"
#include "include/StandardRfAlgo.h"
#include <PllTree.hpp>
#include <fstream>

// boost includes
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>

void RfMetricInterface::do_magical_high_performance_stuff() {
	BOOST_LOG_SEV(logger, lg::critical) << "Everything crumbles, shoot me now!";
	BOOST_LOG_SEV(logger, lg::normal) << "Program is based on commit " << GIT_REVISION;
	std::vector<PllTree> tree_list;
	try {
		tree_list = Util::create_all_trees(parameters.input_file_path);
	} catch (const std::invalid_argument &ex) {
		BOOST_LOG_SEV(logger, lg::error) << ex.what();
		throw;
	}
	switch (parameters.metric) {
		case Metric::RF: {
			StandardRfAlgo standardAlgo;
			result_ptr = std::make_unique<Results>(standardAlgo.calculate(tree_list));
			break;
		}
		default:
			BOOST_LOG_SEV(logger, lg::critical) << "Other metrics not yet implemented!";
			throw std::invalid_argument("This metric has not been implemented");
	}
	has_calculated = true;
}
void RfMetricInterface::disable_logging() {
	lg::LoggingBackend::disable_logging();
}
RfMetricInterface::RfMetricInterface(RfMetricInterface::Params params)
    : parameters(std::move(params)) {
	// optionally provide a tag
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("INTERFACE"));
}
RfMetricInterface::Results &RfMetricInterface::get_result() {
	assert(has_calculated);
	return *result_ptr;
}
bool RfMetricInterface::write_result_to_file() {
	assert(has_calculated);
	std::ofstream out_stream(parameters.output_file_path);
	if (out_stream.is_open()) {
		io::IOData output = get_result_as_IOData();
		nlohmann::json j;
		io::to_json(j, output);

		out_stream << j;
		out_stream.close();
		return true;
	}
	BOOST_LOG_SEV(logger, lg::critical) << "Could not write to outputfile";
	return false;
}
io::IOData RfMetricInterface::get_result_as_IOData() const {
	assert(has_calculated);
	io::IOData output;
	output.pairwise_distance_mtx = result_ptr->pairwise_distances_relative.to_vector();
	output.mean_rf_dst = result_ptr->mean_distance;
	output.number_of_unique_trees = result_ptr->num_unique_trees;
	return output;
}

RfMetricInterface::Results::Results(size_t num_trees)
    : pairwise_distances_absolute(num_trees), pairwise_distances_relative(num_trees) {
}
