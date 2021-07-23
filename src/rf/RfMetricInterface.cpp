
#include "RfMetricInterface.h"
#include "MciAlgo.h"
#include "MsiAlgo.h"
#include "SpiAlgo.h"
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
	if (parameters.threads > 1) {
		BOOST_LOG_SEV(logger, lg::warning)
		    << "No support for multi-core processing yet. Continuing with singular core.";
	}
	try {
		tree_list = Util::create_all_trees_from_file(parameters.input_file_path);
	} catch (const std::invalid_argument &ex) {
		BOOST_LOG_SEV(logger, lg::error) << ex.what();
		throw;
	}
	switch (parameters.metric) {
		case Metric::RF: {
			StandardRfAlgo standardAlgo;
			result_ptr = std::make_unique<Results>(standardAlgo.calculate(tree_list, parameters));
			break;
		}
		case Metric::MCI: {
			MciAlgo genAlgo;
			result_ptr = std::make_unique<Results>(genAlgo.calculate(tree_list, parameters));
			break;
		}
		case Metric::MSI: {
			MsiAlgo genAlgo;
			result_ptr = std::make_unique<Results>(genAlgo.calculate(tree_list, parameters));
			break;
		}
		case Metric::SPI: {
			SpiAlgo genAlgo;
			result_ptr = std::make_unique<Results>(genAlgo.calculate(tree_list, parameters));
			break;
		}
	}
	result_ptr->metric = parameters.metric;
	result_ptr->measure = parameters.measure;
	BOOST_LOG_SEV(logger, lg::notification) << "Calculated results.";
	has_calculated = true;
}
void RfMetricInterface::disable_logging() {
	lg::LoggingBackend::disable_logging();
}
RfMetricInterface::RfMetricInterface(const RfMetricInterface::Params &params) : parameters(params) {
	// optionally provide a tag
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("INTERFACE"));
}
RfMetricInterface::Results &RfMetricInterface::get_result() {
	assert(has_calculated);
	return *result_ptr;
}

// Clang tidy wants to make this const. This isn't possible because of the Logger
bool RfMetricInterface::write_result_to_file() {
	assert(has_calculated);
	std::ofstream out_stream(parameters.output_file_path);
	if (out_stream.is_open()) {
		io::IOData output = get_result_as_IOData();
		nlohmann::json j;
		io::to_json(j, output);
		std::string pretty_format = j.dump(2);

		out_stream << pretty_format;
		out_stream.close();
		return true;
	}
	BOOST_LOG_SEV(logger, lg::critical) << "Could not write to outputfile";
	return false;
}
io::IOData RfMetricInterface::get_result_as_IOData() const {
	assert(has_calculated); // WTF Clang Tidy
	io::IOData output;

	if(result_ptr->metric == Metric::RF){
        output.mean = result_ptr->mean_distance;
        output.number_of_unique_trees = result_ptr->num_unique_trees;
        output.pairwise_tree_score = result_ptr->pairwise_distances.to_vector();
    }else {
		if (result_ptr->measure == Measure::SIMILARITY) {
			output.measure = io::IOData::SIMILARITY;
			output.mean = result_ptr->mean_similarity;
			output.pairwise_tree_score = result_ptr->pairwise_similarities.to_vector();
		} else {
			output.measure = io::IOData::DISTANCE;
			output.pairwise_tree_score = result_ptr->pairwise_distances.to_vector();
			output.mean = result_ptr->mean_distance;
		}
	}
    output.git_revision = Util::get_git_commit();
	const std::string command = "lscpu";
	output.cpuInformation = Util::get_output_of_bash_command(command);
	return output;
}

RfMetricInterface::Results::Results(size_t num_trees)
    : pairwise_distances(num_trees), pairwise_similarities(num_trees) {
}
