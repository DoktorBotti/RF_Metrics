#include "PllSplits.hpp"
#include "PllTree.hpp"

/*  This is an example function. It is _slow_. You should replace it */
size_t PllSplit::popcount(size_t len) {
  size_t popcount = 0;
  for (size_t index = 0; index < len * splitBitWidth(); ++index) {
    if (bitExtract(index) == 1) { popcount += 1; }
  }
  return popcount;
}

uint32_t PllSplit::bitExtract(size_t bit_index) {
  pll_split_base_t split_part = _split[computeMajorIndex(bit_index)];
  return (split_part & (1u << computeMinorIndex(bit_index))) >>
         computeMinorIndex(bit_index);
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
  auto tmp_splits = (pll_split_t)calloc(other.computeSplitArraySize(),
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
