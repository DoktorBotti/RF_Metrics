//
// Created by Robert Schmoltzi on 27.04.21.
//

#include "Util.h"
#include <PllTree.hpp>
#include <algorithm>
#include <sstream>
#include <string>

std::string Util::get_output_of_bash_command(const std::string &cmd) {
    std::string data;
    FILE *stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    auto manipCommand = cmd;
    manipCommand.append(" 2>&1");

    stream = popen(manipCommand.c_str(), "r");

    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != nullptr)
                data.append(buffer);
        pclose(stream);
    }
    return data;
}

std::vector<std::string> Util::split(const std::string &str, const char delim) {
    std::vector<std::string> ret;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) { ret.push_back(token); }
    return ret;
}

std::vector<PllSplitList> Util::create_splits(int n,
                                              const std::string &file_name) {
    auto tree_single_str = get_output_of_bash_command(
        "head -n " + std::to_string(n) + " /rf_data/BS/" + file_name);
    auto tree_str = Util::split(tree_single_str, '\n');

    std::vector<PllTree> trees;
    trees.reserve(tree_str.size());
    for (auto s : tree_str) { trees.emplace_back(s); }

    std::vector<PllSplitList> split_lists;
    split_lists.reserve(trees.size());
    for (auto &t : trees) {
        t.alignNodeIndices(*trees.begin());
        split_lists.emplace_back(t);
    }

    return split_lists;
}
