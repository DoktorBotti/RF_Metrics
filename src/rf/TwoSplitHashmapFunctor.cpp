#include "HashmapUtil/TwoSplitHashmapFunctor.h"

std::size_t TwoSplitHashmapFunctor::operator()(const TwoSplitHashmapKey &key) const {
	size_t hash = 0;
	for (size_t i = 1; i < PllSplit::split_len; ++i) {
		hash ^= key.split_a()[i] ^ key.split_b()[i];
	}
	return hash;
};
