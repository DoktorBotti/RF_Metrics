//
// Created by tbwsl on 4/25/21.
//

#ifndef INFORF_RFMETRICINTERFACE_H
#define INFORF_RFMETRICINTERFACE_H

#include "LoggingBackend.h"
#include "PllTree.hpp"
#include "SymmetricMatrix.hpp"
#include <cstdio>
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
    struct Results {
		explicit Results(size_t num_trees);

		SymmetricMatrix<double> pairwise_distances;
		size_t num_unique_trees = 0;
		double mean_distance = NAN;
	};
	// This should be our class which we expose to others. Like our commandline
	// module
	static Results do_magical_high_performance_stuff(const Params &params);

	void disable_logging();

  private:
	lg::LoggingBackend logging_backend;
	static std::vector<PllTree> create_all_trees(const std::string &file_name);
};

#endif // INFORF_RFMETRICINTERFACE_H
