#pragma once
extern "C" {
#include "libpll/pll.h"
#include "libpll/pll_tree.h"
}

#include <cstddef>
#include <stdexcept>
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
    PllSplit(pll_split_t s, size_t split_length) : _split{s} {
        pre_calc_popcount = popcount(split_length);
    }

    explicit PllSplit(pll_split_t s) : pre_calc_popcount(0), _split{s} {
    }


	size_t popcount_count = 0; // Count Count beware!
	size_t operation_count = 0;
	size_t not_count = 0; // Don't count on me!
	size_t pre_calc_popcount;

	static size_t split_len;

	pll_split_t operator()() const {
		return _split;
	}

	[[nodiscard]] size_t popcount(size_t len); // TODO: make const after testing

	[[nodiscard]] uint32_t bit_extract(size_t bit_index) const;

	[[nodiscard]] bool equals(const PllSplit &other, size_t len);

	/* Trivial and operation. Not optimized as of now. */
	void intersect(const PllSplit &other, size_t len, pll_split_base_t *res);

	/* Trivial or operation. Not optimized as of now. */
	void set_union(const PllSplit &other, size_t len, pll_split_base_t *res);

	/* Trivial xor operation. Not optimized as of now. */
	void set_minus(const PllSplit &other, size_t len, pll_split_base_t *res);

	/* Trivial not operation. Not optimized as of now. */
	void set_not(size_t len, pll_split_base_t *res);

	[[nodiscard]] bool is_disjoint(const PllSplit &other, size_t len);

	[[nodiscard]] static constexpr size_t splitBitWidth() {
		return sizeof(pll_split_base_t) * 8;
	}

  private:
	[[nodiscard]] static constexpr size_t computeMajorIndex(size_t index) {
		return index / splitBitWidth();
	}

	[[nodiscard]] static constexpr size_t computeMinorIndex(size_t index) {
		return index % splitBitWidth();
	}

	pll_split_t _split;
};

class PllSplitList {
  public:
	explicit PllSplitList(const PllTree &tree);

	/* Rule of 5 constructors/destructors */
	~PllSplitList();

	PllSplitList(const PllSplitList &other);

	PllSplitList(PllSplitList &&other) noexcept
	    : _splits(std::exchange(other._splits, {})), _tree_id(other._tree_id) {
	}

	PllSplitList &operator=(const PllSplitList &other) {
		*this = PllSplitList(other);
		return *this;
	}

	PllSplitList &operator=(PllSplitList &&other) noexcept {
		std::swap(_splits, other._splits);
		return *this;
	}

	PllSplit &operator[](size_t index) {
		return _splits[index];
	}

	const PllSplit &operator[](size_t index) const {
		return _splits[index];
	}

	[[nodiscard]] size_t size() const {
		return _splits.size();
	}

	/* Computes the number of pll_split_base_t's that are needed to store a
	 * single split
	 */
	[[nodiscard]] size_t computeSplitLen() const {
		size_t tip_count = _splits.size() + 3;
		size_t split_len = (tip_count / computeSplitBaseSize());

		if ((tip_count % computeSplitBaseSize()) > 0) {
			split_len += 1;
		}

		return split_len;
	}
	[[nodiscard]] PllSplit const *getPtrToNthElem(size_t i) const {
		return &_splits.at(i);
	}

	[[nodiscard]] size_t inline getTreeId() const {
		return _tree_id;
	}
	void inline setTreeId(size_t id) {
		_tree_id = id;
	}

  private:
	/* Computes the number of bits per split base */
	static constexpr size_t computeSplitBaseSize() {
		return sizeof(pll_split_base_t) * 8;
	}

	[[nodiscard]] size_t computeSplitArraySize() const {
		return computeSplitLen() * _splits.size();
	}

	std::vector<PllSplit> _splits;
	size_t _tree_id = 0;
};
