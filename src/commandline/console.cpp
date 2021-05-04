//
// Created by Robert Schmoltzi on 22.04.21.
//

#include "RfMetricInterface.h"
#include <functional>
#include <iostream>

struct Flag {
	std::string name;
	int parameter_count;
	std::string description;
	std::function<void(RfMetricInterface::Params &params, std::vector<std::string>)> func;

	Flag(std::string name,
	     int parameter_count,
	     std::string description,
	     std::function<void(RfMetricInterface::Params &, std::vector<std::string>)> func)
	    : name(move(name)), parameter_count(parameter_count), description(move(description)),
	      func(move(func)) {
	}

	Flag(std::string name,
	     std::string description,
	     const std::function<void(RfMetricInterface::Params &)> &func)
	    : name(move(name)), parameter_count(0), description(move(description)),
	      func([=](RfMetricInterface::Params &params, const std::vector<std::string> &) {
		      func(params);
	      }) {
	}
};

std::vector<Flag> flags = {
    {"--metric",
     1,
     "[ RF | MCI | MSI | SPI ] The metric to be calculated.",
     [](RfMetricInterface::Params &params, std::vector<std::string> args) {
	     if (args[0] == "RF") {
		     params.metric = RfMetricInterface::Metric::RF;
	     } else if (args[0] == "MCI") {
		     params.metric = RfMetricInterface::Metric::MCI;
	     } else if (args[0] == "MSI") {
		     params.metric = RfMetricInterface::Metric::MSI;
	     } else if (args[0] == "SPI") {
		     params.metric = RfMetricInterface::Metric::SPI;
	     } else {
		     throw std::invalid_argument("Metric must be RF, MCI, MSI or SPI.");
	     }
     }},
    {"-i",
     1,
     "Input file path",
     [](RfMetricInterface::Params &params, std::vector<std::string> args) {
	     params.input_file_path = args[0];
     }},
    {"-o",
     1,
     "Output file path",
     [](RfMetricInterface::Params &params, std::vector<std::string> args) {
	     params.output_file_path = args[0];
     }},
    {"-t", 1, "Thread count", [](RfMetricInterface::Params &params, std::vector<std::string> args) {
	     params.threads = std::stoul(args[0]);
     }}};

int main(int argc, char *argv[]) {
	try {
		if (argc == 1) {
			std::cout << "Super RF metrics is in the house. House is full. For details type:\n\n\t"
			          << argv[0] << " help\n"
			          << std::endl;
			return 0;
		}

		sort(std::begin(flags), std::end(flags), [](const Flag &l, const Flag &r) {
			return l.name < r.name;
		});

		bool flag_found;
		int arg_pos = 1;
		RfMetricInterface::Params params;
		while (arg_pos != argc) {
			flag_found = false;
			for (auto &c : flags) {
				if (c.name == argv[arg_pos]) {
					if (arg_pos + 1 + c.parameter_count > argc)
						throw std::runtime_error("Not enough parameters to command " + c.name);

					std::vector<std::string> args(argv + arg_pos + 1,
					                              argv + arg_pos + 1 + c.parameter_count);
					++arg_pos;
					arg_pos += c.parameter_count;

					c.func(params, move(args));

					flag_found = true;
				}
			}
			if (!flag_found) {
				throw std::runtime_error(std::string("Unknown flag ") + argv[arg_pos]);
			}
		}

		 auto res = RfMetricInterface(params);
		 res.do_magical_high_performance_stuff();
		 bool success = res.write_result_to_file();
		 assert(success);

	} catch (const std::exception &err) {
		std::cerr << "Exception : " << err.what() << std::endl;
		throw err;
	}
}