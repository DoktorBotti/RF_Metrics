#include "PllSplits.hpp"
#include "PllTree.hpp"

// extern "C" int _mm_popcnt_u32(unsigned int a);

/*  Calculates the Hamming weight of the split. */
size_t PllSplit::popcount(size_t len) const {
    // Ideas for popcount:
    // _mm_popcnt_u32() with IFDEFs for fallback -> #include x86intrin.h
    // __builtin_popcount(), the clang builtin

    size_t popcount = 0;
    for (size_t i = 0; i < len; i++) {
        popcount += __builtin_popcount(_split[i]);
        //popcount += _mm_popcnt_u32(_split[i]);
    }
    return popcount;
}

uint32_t PllSplit::bitExtract(size_t bit_index) const {
    pll_split_base_t split_part = _split[computeMajorIndex(bit_index)];
    return (split_part & (1u << computeMinorIndex(bit_index))) >> computeMinorIndex(bit_index);
}

PllSplitList::PllSplitList(const PllTree &tree) {
    auto tmp_splits = pllmod_utree_split_create(
            tree.tree()->vroot, tree.tree()->tip_count, nullptr);

    for (size_t i = 0; i < tree.tree()->tip_count - 3; ++i) {
        _splits.emplace_back(tmp_splits[i]);
    }

    free(tmp_splits);
}

PllSplitList::PllSplitList(const PllSplitList &other) {
    auto tmp_splits = (pll_split_t) calloc(other.computeSplitArraySize(),
                                           sizeof(pll_split_base_t));

    memcpy(tmp_splits,
           other._splits[0](),
           other.computeSplitArraySize() * sizeof(pll_split_base_t));

    for (size_t i = 0; i < other.computeSplitArraySize(); ++i) {
        _splits.emplace_back(tmp_splits + (i * other.computeSplitLen()));
    }
}

PllSplitList::~PllSplitList() {
    if (!_splits.empty()) { free(_splits[0]()); }
}
