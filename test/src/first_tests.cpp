#include <catch2/catch.hpp>
#include "PllSplits.hpp"
#include "PllTree.hpp"
#include <string>

TEST_CASE("Catch2 integration", "[useless]"){
  REQUIRE(2+2 == 4);
}
TEST_CASE("sample code", "[useless]"){
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
