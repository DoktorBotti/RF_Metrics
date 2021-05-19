//
// Created by Robert Schmoltzi on 27.04.21.
//

#include "Util.h"
#include <PllTree.hpp>
#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>
#include <string>

std::string Util::get_output_of_bash_command(const std::string & cmd) {
	std::string data;
	FILE * stream;
	const int max_buffer = 256;
	auto manipCommand = cmd;
	manipCommand.append(" 2>&1");

	stream = popen(manipCommand.c_str(), "r");

	if (stream) {
        char buffer[max_buffer];
		while (!feof(stream))
			if (fgets(buffer, max_buffer, stream) != nullptr)
				data.append(buffer);
		pclose(stream);
	}
	return data;
}

std::vector<std::string> Util::split(const std::string & str, const char delim) {
	std::vector<std::string> ret;
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim)) {
		ret.push_back(token);
	}
	return ret;
}

std::vector<PllSplitList> Util::create_splits(int n, const std::string & file_name) {
	auto trees = create_trees(n, file_name);

	std::vector<PllSplitList> split_lists;
	split_lists.reserve(trees.size());
	for (auto & t : trees) {
		t.alignNodeIndices(*trees.begin());
		split_lists.emplace_back(t);
	}
	return split_lists;
}
std::vector<PllTree> Util::create_trees(int n, const std::string & file_name) {
	auto tree_single_str = get_output_of_bash_command("head -n " + std::to_string(n) + " " + file_name);
	auto tree_str = Util::split(tree_single_str, '\n');

	return get_tree_from_string_list(tree_str);
}
std::vector<PllTree> Util::get_tree_from_string_list(std::vector<std::string> &tree_str) {
	std::vector<PllTree> trees(tree_str.begin(), tree_str.end());
	return trees;
}
std::vector<PllTree> Util::create_all_trees(const std::string &file_name) {
    std::ifstream overview(file_name);
    if (!overview.is_open()) {
        throw std::invalid_argument("Could not open input file");
    }
    // read entire file to string
    std::string file_content((std::istreambuf_iterator<char>(overview)),
                             (std::istreambuf_iterator<char>()));
    overview.close();
    auto tree_str = Util::split(file_content, '\n');
	return get_tree_from_string_list(tree_str);
}
SymmetricMatrix<double> Util::create_random_mtx(const size_t dim) {
    SymmetricMatrix<double> res(dim);
	std::random_device dev;
    std::mt19937 rand_engine(dev());
    std::uniform_real_distribution<> distr(0., 500.);

    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j <= i; ++j) {
            res.set_at(i, j, distr(rand_engine));
        }
    }
    return res;
}
