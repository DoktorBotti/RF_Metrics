#pragma once
extern "C" {
#include "libpll/pll.h"
#include "libpll/pll_tree.h"
}

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

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
	explicit PllSplit(pll_split_t s) : _split{s} {
	}
	PllSplit() : _split(nullptr){}
	bool operator==(const PllSplit &rhs) const;
	bool operator!=(const PllSplit &rhs) const;
	static size_t split_len;

	pll_split_t operator()() const {
		return _split;
	}
	PllSplit(const PllSplit&) = default;
	PllSplit(PllSplit&&) = default;
	PllSplit& operator=(const PllSplit&)= default;
	~PllSplit() = default;

	void perform_popcount_precalc(size_t split_len) const;

	[[nodiscard]] size_t popcount(size_t len) const;
	[[nodiscard]] size_t getPrecalcPopcnt() const;

	[[nodiscard]] uint32_t bit_extract(size_t bit_index) const;

	[[nodiscard]] bool equals(const PllSplit &other, size_t len) const;

	/* Trivial and operation. Not optimized as of now. */
	void intersect(const PllSplit &other, size_t len, pll_split_base_t *res) const;

	/* Trivial or operation. Not optimized as of now. */
	void set_union(const PllSplit &other, size_t len, pll_split_base_t *res) const;

	/* Trivial xor operation. Not optimized as of now. */
	void set_minus(const PllSplit &other, size_t len, pll_split_base_t *res) const;

	/* Trivial not operation. Not optimized as of now. */
	void set_not(size_t len, pll_split_base_t *res) const;

	[[nodiscard]] bool is_disjoint(const PllSplit &other, size_t len) const;

	[[nodiscard]] static constexpr size_t splitBitWidth() {
		return sizeof(pll_split_base_t) * 8;
	}

	void setIntersectionIdx(size_t idx);
	size_t getScoreIndex() const;
	double getHInfoContent() const;
	void setHInfoContent(double val) const;

	bool operator<(PllSplit const &rhs) const;
	bool operator>(PllSplit const &rhs) const;

  private:
	[[nodiscard]] static constexpr size_t computeMajorIndex(size_t index) {
		return index / splitBitWidth();
	}

	[[nodiscard]] static constexpr size_t computeMinorIndex(size_t index) {
		return index % splitBitWidth();
	}
    bool inline comparePrequesites(const PllSplit& rhs) const;

        size_t priv_popcount(size_t len) const;

	pll_split_t _split;

	// precalculations and optimization variables
	size_t intersection_matrix_index = 0;
    mutable size_t precalc_popcount = 0;
    mutable double precalc_h_info_content = 0;
};
