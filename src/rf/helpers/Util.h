//
// Created by Robert Schmoltzi on 27.04.21.
//

#ifndef CONCURRENT_GROWTABLE_UTIL_H
#define CONCURRENT_GROWTABLE_UTIL_H

#include "PllTree.hpp"
#include "RectMatrix.hpp"
#include "SymmetricMatrix.hpp"
#include <PllSplit.hpp>
#include <string>
#include <vector>

class Util {
  public:
	/* Takes a string and executes it as a bash command. The output is returned
	 * as string.*/
	static std::string get_output_of_bash_command(const std::string &cmd);
	/* Definitely should not be part of any standard library */
	static std::vector<std::string> split(const std::string &str, char delim);
	/* Create the PllSplitLists of the first n trees in the specified file. */
	static std::vector<PllSplitList> create_splits(int n, const std::string &file_name);
	static std::vector<PllTree> create_trees(int n, const std::string &file_name);
	static std::vector<PllTree> create_all_trees_from_file(const std::string &file_name);
	static std::vector<PllTree> create_all_trees_from_string(const std::string &trees);
	static RectMatrix<double>
	parse_mtx_from_r(const std::string &file_path, char delim_row = '\n', char delim_col = ',');
	static SymmetricMatrix<double>
	parse_sym_mtx_from_r(const std::string &file_path, char delim_row = '\n', char delim_col = ',');

	static RectMatrix<double> create_random_mtx(const size_t dim);

	static std::string read_file(const std::string &file_name);

  private:
	static std::vector<PllTree> get_tree_from_string_list(std::vector<std::string> &tree_str);
};

#endif // CONCURRENT_GROWTABLE_UTIL_H
