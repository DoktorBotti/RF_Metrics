//
// Created by Robert Schmoltzi on 22.04.21.
//

#include "RfMetricInterface.h"
#include <functional>
#include <iostream>
#include <array>

struct Flag {
	std::string name;
	int parameter_count;
	std::string description;
	std::function<void(RfMetricInterface::Params &params, std::vector<std::string>)> func;

	Flag(std::string name_in,
	     int parameter_count_in,
	     std::string description_in,
	     std::function<void(RfMetricInterface::Params &, std::vector<std::string>)> func_in)
	    : name(move(name_in)), parameter_count(parameter_count_in), description(move(description_in)),
	      func(move(func_in)) {
	}

	Flag(std::string name_in,
	     std::string description_in,
	     const std::function<void(RfMetricInterface::Params &)> &func_in)
	    : name(move(name_in)), parameter_count(0), description(move(description_in)),
	      func([=](RfMetricInterface::Params &params, const std::vector<std::string> &) {
		      func_in(params);
	      }) {
	}
};

const static std::vector<Flag> flags = {
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

int main(int argc, char **argv) {
		if (argc == 1) {
			std::cout << "Please use the format: " << argv[0]
			          << " --metric [ RF | MCI | MSI | SPI ]"
			          << " -i [input-file-path]"
			          << " -o [output-file-path]" << std::endl;
			return 0;
		}
		bool flag_found = false;
		int arg_pos = 1;
		RfMetricInterface::Params params;
		while (arg_pos < argc) {
			flag_found = false;
			for (auto &c : flags) {
				if (!flag_found && c.name == argv[arg_pos]) {
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
	    return 0;

}