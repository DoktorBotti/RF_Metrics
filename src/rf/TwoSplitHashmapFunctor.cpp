#include "HashmapUtil/TwoSplitHashmapFunctor.h"

std::size_t TwoSplitHashmapFunctor::operator()(const TwoSplitHashmapKey &key) const {
	//EEEEEEEEEEEEVVVVVVVVVVIIIIIIIIILLLLLLLLL
    return key.pre_hash;
};
