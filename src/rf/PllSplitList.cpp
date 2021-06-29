#include "PllSplitList.h"
#include "PllTree.hpp"

PllSplitList::PllSplitList(const PllTree &tree) {
    auto tmp_splits =
        pllmod_utree_split_create(tree.tree()->vroot, tree.tree()->tip_count, nullptr);

    for (size_t i = 0; i < tree.tree()->tip_count - 3; ++i) {
        _splits.emplace_back(tmp_splits[i]);
    }

    free(tmp_splits);
}

PllSplitList::PllSplitList(const PllSplitList &other) {
    auto tmp_splits =
        static_cast<pll_split_t>(calloc(other.computeSplitArraySize(), sizeof(pll_split_base_t)));
    _tree_id = other._tree_id;
    memcpy(
        tmp_splits, other._splits[0](), other.computeSplitArraySize() * sizeof(pll_split_base_t));

    for (size_t i = 0; i < other.computeSplitArraySize(); ++i) {
        _splits.emplace_back(tmp_splits + (i * other.computeSplitLen()));
    }
}

PllSplitList::~PllSplitList() {
    if (!_splits.empty()) {
        free(_splits[0]()); // Probably fine. Always allocate all splits in a single chunk of
        // memory!!
    }
}
