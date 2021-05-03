//
// Created by tbwsl on 4/25/21.
//

#include "RfMetricInterface.h"
#include "gitVersionHeader.h"
// the following two suffice for severity
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>

// needed to add a tag
#include <PllTree.hpp>
#include <boost/log/attributes/constant.hpp>

void RfMetricInterface::do_magical_high_performance_stuff(
    const RfMetricInterface::Params &params) {
    // ideally this is a member of a class.
    boost::log::sources::severity_logger<lg::SeverityLevel> logger;
    // optionally provide a tag
    logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("Foobar"));

    BOOST_LOG_SEV(logger, lg::critical) << "Everything crumbles, shoot me now!";
    BOOST_LOG_SEV(logger, lg::normal)
        << "Program is based on commit " << GIT_REVISION;


    std::vector<PllTree> tree_list = RfMetricInterface::create_all_trees( params.input_file_path);

}
void RfMetricInterface::disable_logging() { logging_backend.disable_logging(); }
std::vector<PllTree> RfMetricInterface::create_all_trees(const std::string & file_name) {
	return std::vector<PllTree>();
}
