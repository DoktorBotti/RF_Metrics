#include "Util.h"
#include "gitVersionHeader.h"
#include <PllTree.hpp>
#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>
#include <string>

std::string Util::get_output_of_bash_command(const std::string &cmd) {
	std::string data;
	FILE *stream;
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

std::vector<std::string> Util::split(const std::string &str, const char delim) {
	std::vector<std::string> ret;
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim)) {
		ret.push_back(token);
	}
	return ret;
}

std::vector<PllSplitList> Util::create_splits(int n, const std::string &file_name) {
	auto trees = create_trees(n, file_name);

	std::vector<PllSplitList> split_lists;
	split_lists.reserve(trees.size());
	for (auto &t : trees) {
		t.alignNodeIndices(*trees.begin());
		split_lists.emplace_back(t);
	}
	return split_lists;
}
std::vector<PllTree> Util::create_trees(int n, const std::string &file_name) {
	auto tree_single_str =
	    get_output_of_bash_command("head -n " + std::to_string(n) + " " + file_name);
	auto tree_str = Util::split(tree_single_str, '\n');

	return get_tree_from_string_list(tree_str);
}
std::vector<PllTree> Util::get_tree_from_string_list(std::vector<std::string> &tree_str) {
	std::vector<PllTree> trees(tree_str.begin(), tree_str.end());
	return trees;
}
std::vector<PllTree> Util::create_all_trees_from_file(const std::string &file_name) {
	std::string file_content = read_file(file_name);
	auto tree_str = Util::split(file_content, '\n');
	return get_tree_from_string_list(tree_str);
}
std::string Util::read_file(const std::string &file_name) {
	std::ifstream ifstr(file_name);
	if (!ifstr.is_open()) {
		throw std::invalid_argument("Could not open input file");
	}
	// read entire file to string
	std::string file_content((std::istreambuf_iterator<char>(ifstr)),
	                         (std::istreambuf_iterator<char>()));
	ifstr.close();
	return file_content;
}
RectMatrix<double> Util::create_random_mtx(const size_t dim) {
	RectMatrix<double> res(dim);
	std::random_device dev;
	std::mt19937 rand_engine(dev());
	std::uniform_real_distribution<> distr(0., 500.);

	for (size_t i = 0; i < dim; ++i) {
		for (size_t j = 0; j < dim; ++j) {
			res.set(i, j, distr(rand_engine));
		}
	}
	return res;
}

RectMatrix<double>
Util::parse_mtx_from_r(const std::string &file_path, char delim_row, char delim_col) {
	std::vector<std::vector<double>> dbl_mat = parseMat(file_path, delim_row, delim_col);

	// copy out matrix
	size_t dim = dbl_mat.size();
	RectMatrix<double> res(dim);
	for (size_t i = 0; i < dim; ++i) {
		for (size_t j = 0; j < dim; ++j) {
			res.set(i, j, dbl_mat[i][j]);
		}
	}
	return res;
}
std::vector<std::vector<double>>
Util::parseMat(const std::string &file_path, char rows, char cols) {
	std::string file_content = read_file(file_path);
	auto lines = split(file_content, rows);
	std::vector<std::vector<std::string>> mat;
	mat.reserve(lines.size());
	for (const auto &el : lines) {
		mat.push_back(split(el, cols));
	}

	std::vector<std::vector<double>> dbl_mat;
	for (const auto &el : mat) {
		dbl_mat.emplace_back();
		std::for_each(el.begin(), el.end(), [&dbl_mat](auto elem) {
			dbl_mat.back().push_back(std::stod(elem));
		});
	}
	return dbl_mat;
}

SymmetricMatrix<double>
Util::parse_sym_mtx_from_r(const std::string &file_path, char delim_row, char delim_col) {
    std::vector<std::vector<double>> dbl_mat = parseMat(file_path, delim_row, delim_col);

    // copy out matrix
	size_t dim = dbl_mat.size();
	SymmetricMatrix<double> res(dim);
	for (size_t i = 0; i < dim; ++i) {
		for (size_t j = 0; j <= i; ++j) {
			res.set_at(i, j, dbl_mat[i][j]);
		}
	}
	return res;
}
std::vector<PllTree> Util::create_all_trees_from_string(const std::string &trees) {
	auto tree_str = Util::split(trees, '\n');
	return get_tree_from_string_list(tree_str);
}
std::string Util::get_git_commit() {
	return GIT_REVISION;
}
