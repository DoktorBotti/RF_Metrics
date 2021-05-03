#include "PllTree.hpp"
#include "PllSplits.hpp"
#include "libpll/pll.h"
#include <stdexcept>

PllTree::PllTree(const std::string &newick_string) {
	_tree = pll_utree_parse_newick_string_unroot(newick_string.c_str());
	if (_tree == nullptr) {
		throw std::invalid_argument{"A tree falied to parse"};
	}
}

PllTree::PllTree(const PllTree &other) {
	_tree = pll_utree_clone(other._tree);
}

PllTree::~PllTree() {
	pll_utree_destroy(_tree, nullptr);
}

PllSplitList PllTree::makeSplits() const {
	return PllSplitList(*this);
}

void PllTree::alignNodeIndices(const PllTree &other) {
	/* Bad things happen if we call this function with the same tree twice */
	if (this == &other || _tree == other._tree) {
		return;
	}

	auto success = pllmod_utree_consistency_set(other._tree, _tree);
	if (success != PLL_SUCCESS) {
		throw std::runtime_error{pll_errmsg};
	}
}
