#include "PllSplits.hpp"
#include "PllTree.hpp"
#include "libpll/pll.h"
#include <stdexcept>

PllTree::PllTree(const std::string &newick_string) {
  _tree = pll_utree_parse_newick_string_unroot(newick_string.c_str());
  if (_tree == nullptr) {
    throw std::invalid_argument{"A tree falied to parse"};
  }
}

PllTree::PllTree(const PllTree &other) { _tree = pll_utree_clone(other._tree); }

PllTree::~PllTree() { pll_utree_destroy(_tree, nullptr); }

PllSplits PllTree::makeSplits() const { return PllSplits(*this); }
