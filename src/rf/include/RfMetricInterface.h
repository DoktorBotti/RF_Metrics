//
// Created by tbwsl on 4/25/21.
//

#ifndef INFORF_RFMETRICINTERFACE_H
#define INFORF_RFMETRICINTERFACE_H

#include "LoggingBackend.h"
#include "PllTree.hpp"
#include <cstdio>
#include <string>
#include <vector>
class RfMetricInterface {
  public:
	struct Params {
		size_t threads;
		std::string input_file_path;
		std::string output_file_path;
		enum { RF, MCI, MSI, SPI } metric;
	};
	// This should be our class which we expose to others. Like our commandline
	// module
	static void do_magical_high_performance_stuff(const Params & params);

	void disable_logging();

  private:
	lg::LoggingBackend logging_backend;
	static std::vector<PllTree> create_all_trees(const std::string & file_name);
};

#endif // INFORF_RFMETRICINTERFACE_H
