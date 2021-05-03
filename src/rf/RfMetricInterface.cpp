//
// Created by tbwsl on 4/25/21.
//

// the following two suffice for severity
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>

// needed to add a tag
#include <boost/log/attributes/constant.hpp>
#include <fstream>

#include "RfMetricInterface.h"
#include "gitVersionHeader.h"
#include "include/StandardRfAlgo.h"
#include <PllTree.hpp>
#include "helpers/Util.h"
RfMetricInterface::Results
RfMetricInterface::do_magical_high_performance_stuff(const RfMetricInterface::Params &params) {
	// ideally this is a member of a class.
	boost::log::sources::severity_logger<lg::SeverityLevel> logger;
	// optionally provide a tag
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("Foobar"));

	BOOST_LOG_SEV(logger, lg::critical) << "Everything crumbles, shoot me now!";
	BOOST_LOG_SEV(logger, lg::normal) << "Program is based on commit " << GIT_REVISION;
	std::vector<PllTree> tree_list;
	try {
		tree_list = Util::create_all_trees(params.input_file_path);
	} catch (const std::invalid_argument &ex) {
		BOOST_LOG_SEV(logger, lg::error) << ex.what();
		throw ex;
	}
	Results results(tree_list.size());
	switch (params.metric) {
		case Metric::RF: {
			StandardRfAlgo standardAlgo;
			standardAlgo.calculate(tree_list);
			break;
		}
		default:
			BOOST_LOG_SEV(logger, lg::critical) << "Other metrics not yet implemented!";
			throw std::invalid_argument("This metric has not been implemented");
			break;
	}
	return results;
}
void RfMetricInterface::disable_logging() {
	logging_backend.disable_logging();
}
RfMetricInterface::Results::Results(size_t num_trees) : pairwise_distances(num_trees) {
}
