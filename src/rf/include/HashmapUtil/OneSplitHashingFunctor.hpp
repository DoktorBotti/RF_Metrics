#ifndef INFORF_HASHINGFUNCTOR_HPP
#define INFORF_HASHINGFUNCTOR_HPP
#include "OneSplitHashmapKey.hpp"

struct OneSplitHashingFunctor {
	std::size_t operator()(const OneSplitHashmapKey &key) const {
		size_t hash = key.split->operator()()[0];
		for (size_t i = 1; i < PllSplit::split_len; ++i) {
			hash ^= key.split->operator()()[i];
		}
		return hash;
	}
};
#endif // INFORF_HASHINGFUNCTOR_HPP
