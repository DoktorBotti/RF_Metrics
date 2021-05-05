//
// Created by tbwsl on 4/25/21.
//

#ifndef INFORF_RFMETRICINTERFACE_H
#define INFORF_RFMETRICINTERFACE_H

#include "LoggingBackend.h"
#include "PllTree.hpp"
#include "SymmetricMatrix.hpp"

#include <boost/log/sources/severity_logger.hpp>
#include "../../commandline/IO/IOData.h"
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
class RfMetricInterface {
  public:
	enum Metric { RF, MCI, MSI, SPI };
	struct Params {
		size_t threads = 1;
		std::string input_file_path;
		std::string output_file_path;
		Metric metric = Metric::RF;
	};
	explicit RfMetricInterface(Params params);
	struct Results {
		explicit Results(size_t num_trees);
		Results(const Results& ) = default;
		Results(Results&& ) = default;
		~Results() = default;

		SymmetricMatrix<size_t> pairwise_distances_absolute;
		SymmetricMatrix<double> pairwise_distances_relative;
		size_t num_unique_trees = 0;
		double mean_distance = NAN;
	};
	// This should be our class which we expose to others. Like our commandline
	// module
	void do_magical_high_performance_stuff();
	Results &get_result();
	bool write_result_to_file();
	[[nodiscard]] io::IOData get_result_as_IOData() const;
	static void disable_logging();

  private:
	boost::log::sources::severity_logger<lg::SeverityLevel> logger;
	const Params parameters;
	bool has_calculated = false;
	std::unique_ptr<Results> result_ptr;
	lg::LoggingBackend logging_backend;
};

#endif // INFORF_RFMETRICINTERFACE_H
