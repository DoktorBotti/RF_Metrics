#include "include/HashmapUtil/TwoSplitHashmapKey.h"
bool TwoSplitHashmapKey::operator==(const TwoSplitHashmapKey &rhs) const {
	// assumes, that we use FastSplits!
	return split_a() == rhs.split_b() && split_b() == rhs.split_a();
}
bool TwoSplitHashmapKey::operator!=(const TwoSplitHashmapKey &rhs) const {
	return !(rhs == *this);
}
TwoSplitHashmapKey::TwoSplitHashmapKey() : split_a(nullptr), split_b(nullptr) {
}
TwoSplitHashmapKey::TwoSplitHashmapKey(const PllSplit *copy_split_a, const PllSplit *copy_split_b)
{
	if(copy_split_a->operator()() < copy_split_b->operator()()){
		split_a = PllSplit(copy_split_a->operator()());
		split_b = PllSplit(copy_split_b->operator()());
	}else{
        split_a = PllSplit(copy_split_b->operator()());
        split_b = PllSplit(copy_split_a->operator()());
	}
	for(size_t i = 0; i < PllSplit::split_len; ++i){
		pre_hash ^= split_a()[i] ^ split_b()[i];
	}
}
