//
// Created by Robert Schmoltzi on 22.04.21.
//

#include <rf/PllTree.hpp>
#include <string>
#include <vector>

int main() {
  std::vector<std::string> tree_strings{
      "(a, b, (c, d));",
      "(a, d, (c, b));",
  };
  std::vector<PllTree> tree_list;
  for (auto t : tree_strings) { tree_list.emplace_back(t); }

  std::vector<PllSplitList> splits_list;

  for (auto &t : tree_list) {
    t.alignNodeIndices(*tree_list.begin());
    splits_list.emplace_back(t);
  }
}