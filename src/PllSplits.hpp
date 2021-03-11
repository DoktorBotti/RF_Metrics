#pragma once
#include "libpll/pll.h"
#include "libpll/pll_tree.h"
#include <cstddef>
#include <utility>
#include <vector>

class PllTree;

/*
 * A convenience class for the purposes of doing math on the individual splits.
 * The pll_split_t is a non-owning pointer, so this class does not have a
 * destructor. The functions which are probably needed:
 *
 *  - popcount
 *  - bitextract
 *  - intersect (and)
 *  - union (or)
 *  - set minus (xor)
 *
 * I have already written an example bit extract and popcount, but they are far
 * from optimal. Remember that the underlying type is a *pointer* to an array of
 * pll_split_base_t (which is an unsigned int), because the number of taxa in
 * the could be more than 32 (or even 64). This means that you will need to
 * iterate over the array to compute the correct value for popcount etc.
 */
class PllSplit {
public:
  PllSplit(pll_split_t s) : _split{s} {}

  pll_split_t operator()() const { return _split; }

  size_t   popcount(size_t len);
  uint32_t bitExtract(size_t bit_index);

private:
  constexpr size_t splitBitWidth() const {
    return sizeof(pll_split_base_t) * 8;
  }

  constexpr size_t computeMajorIndex(size_t index) {
    return index / splitBitWidth();
  }

  constexpr size_t computeMinorIndex(size_t index) {
    return index % splitBitWidth();
  }

  pll_split_t _split;
};

class PllSplits {
public:
  PllSplits(const PllTree &tree);

  /* Rule of 5 constructors/destructors */
  ~PllSplits();
  PllSplits(const PllSplits &other);
  PllSplits(PllSplits &&other) : _splits{std::exchange(other._splits, {})} {}
  PllSplits &operator=(const PllSplits &other) {
    return *this = PllSplits(other);
  };
  PllSplits &operator=(PllSplits &&other) {
    std::swap(_splits, other._splits);
    return *this;
  };

private:
  /* Computes the number of bits per split base */
  constexpr size_t computSplitBaseSize() const {
    return sizeof(pll_split_base_t) * 8;
  }

  size_t computeSplitLen() const {
    size_t tip_count = _splits.size() + 3;
    size_t split_len = (tip_count / computSplitBaseSize());

    if ((tip_count % computSplitBaseSize()) > 0) { split_len += 1; }

    return split_len;
  }

  size_t computeSplitArraySize() const {
    return computeSplitLen() * _splits.size();
  }

  std::vector<PllSplit> _splits;
};
