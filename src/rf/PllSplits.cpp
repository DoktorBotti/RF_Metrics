#include "PllSplits.hpp"
#include "PllTree.hpp"

/*  Calculates the Hamming weight of the split. */
size_t PllSplit::popcount(size_t len) const {
	size_t popcount = 0;
	for (size_t i = 0; i < len; ++i) {
		// For use of asm( popcnt) use compiler flag -mpopcnt
		//		if constexpr (sizeof(pll_split_base_t) == 4) {
		popcount += static_cast<size_t>(__builtin_popcount(_split[i]));
		//		} else if constexpr (sizeof(pll_split_base_t) == 8) {
		//			popcount += static_cast<size_t>(__builtin_popcountll(_split[i]));
		//		} else {
		//			throw std::invalid_argument("Size of pll_split_base_t must be 4 or 8");
		//		}
	}
	return popcount;
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

size_t PllSplit::split_len = 1337;

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

// ------------- PllSplitList -------------

PllSplitList::PllSplitList(const PllTree &tree) {
	auto tmp_splits =
	    pllmod_utree_split_create(tree.tree()->vroot, tree.tree()->tip_count, nullptr);

    size_t split_len = (tree.tree()->tip_count / computeSplitBaseSize());
    if ((tree.tree()->tip_count % computeSplitBaseSize()) > 0) {
        split_len += 1;
    }

	_splits.reserve(tree.tree()->tip_count - 3);
	for (size_t i = 0; i < tree.tree()->tip_count - 3; ++i) {
		_splits.emplace_back(PllSplit(tmp_splits[i], split_len));
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
		_splits.emplace_back(PllSplit(tmp_splits + (i * other.computeSplitLen()), other.computeSplitLen()));
	}
}

PllSplitList::~PllSplitList() {
	if (!_splits.empty()) {
		free(_splits[0]()); // Probably fine. Always allocate all splits in a single chunk of
		                    // memory!!
	}
}
