#include "PllSplit.hpp"
#include "PllTree.hpp"
#include <GeneralizedRfAlgo.h>

/*  Calculates the Hamming weight of the split. */
size_t PllSplit::popcount(size_t len) const {
	return priv_popcount(len);
}

uint32_t PllSplit::bit_extract(size_t bit_index) const {
	pll_split_base_t split_part = _split[computeMajorIndex(bit_index)];
	return (split_part & (1u << computeMinorIndex(bit_index))) >> computeMinorIndex(bit_index);
}

void PllSplit::intersect(const PllSplit &other, const size_t len, pll_split_base_t *res) const {
	for (size_t i = 0; i < len; ++i) {
		res[i] = _split[i] & other._split[i];
	}
}

bool PllSplit::is_disjoint(const PllSplit &other, size_t len) const {
	for (size_t i = 0; i < len; ++i) {
		if ((_split[i] & other._split[i]) != 0) {
			return false;
		}
	}
	return true;
}

size_t PllSplit::split_len = std::numeric_limits<size_t>::max();

void PllSplit::set_union(const PllSplit &other, size_t len, pll_split_base_t *res) const {
	for (size_t i = 0; i < len; ++i) {
		res[i] = _split[i] | other._split[i];
	}
}

void PllSplit::set_minus(const PllSplit &other, size_t len, pll_split_base_t *res) const {
	for (size_t i = 0; i < len; ++i) {
		res[i] = _split[i] ^ other._split[i];
	}
}

bool PllSplit::equals(const PllSplit &other, size_t len) const {
	for (size_t i = 0; i < len; ++i) {
		if (_split[i] != other._split[i]) {
			return false;
		}
	}
	return true;
}

void PllSplit::set_not(size_t len, pll_split_base_t *res) const {
	for (size_t i = 0; i < len; i++) {
		res[i] = ~_split[i];
	}
}
size_t PllSplit::priv_popcount(size_t len) const {
	size_t popcount = 0;
	for (size_t i = 0; i < len; ++i) {
		// Optimize later for use of asm( popcnt) use compiler flag -mpopcnt
		//		if constexpr (sizeof(pll_split_base_t) == 4) {
		popcount += static_cast<size_t>(__builtin_popcount(_split[i]));
		//		} else if constexpr (sizeof(pll_split_base_t) == 8) {
		//			popcount += static_cast<size_t>(__builtin_popcountll(_split[i]));
		//		} else {
		//			throw std::invalid_argument("Size of pll_split_base_t must be 4 or 8");
		//		}
	}
	precalc_popcount = popcount;
	return popcount;
}
void PllSplit::perform_popcount_precalc(size_t split_len) const {
	priv_popcount(split_len);
}
// requires split_len to be something sensible
bool PllSplit::operator<(const PllSplit &rhs) const {
	if (!operator()() || !rhs()) {
		return false;
	}
	assert(PllSplit::split_len < std::numeric_limits<size_t>::max());
	for (size_t i = 0; i < PllSplit::split_len; ++i) {
		if (operator()()[i] != rhs()[i]) {
			return operator()()[i] < rhs()[i];
		}
	}
	return operator()() < rhs();
}
bool PllSplit::operator==(const PllSplit &rhs) const {
	if (!operator()() || !rhs()) {
		return false;
	}
	for (size_t i = 0; i < PllSplit::split_len; ++i) {
		if (operator()()[i] != rhs()[i]) {
			return false;
		}
	}
	return true;
}
bool PllSplit::operator!=(const PllSplit &rhs) const {
	return !(*this == rhs);
}
bool PllSplit::operator>(const PllSplit &rhs) const {
	if (!operator()() || !rhs()) {
		return false;
	}
	assert(PllSplit::split_len < std::numeric_limits<size_t>::max());
	for (size_t i = 0; i < PllSplit::split_len; ++i) {
		if (operator()()[i] != rhs()[i]) {
			return operator()()[i] > rhs()[i];
		}
	}
	return operator()() > rhs();
}
void PllSplit::setIntersectionIdx(size_t idx) {
	intersection_matrix_index = idx;
}
size_t PllSplit::getScoreIndex() const {
	return intersection_matrix_index;
}
double PllSplit::getHInfoContent() const {
	assert(precalc_h_info_content > 0.);
	return precalc_h_info_content;
}
size_t PllSplit::getPrecalcPopcnt() const {
	assert(precalc_popcount > 0);
	return precalc_popcount;
}
void PllSplit::setHInfoContent(double val) const {
	// this should only happen once -> zero before
	assert(precalc_h_info_content <= 0);

	precalc_h_info_content = val;
}
