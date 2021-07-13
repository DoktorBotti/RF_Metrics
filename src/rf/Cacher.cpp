#include "HashmapUtil//Cacher.h"
bool Cacher::lookupTrees(TwoSplitHashmapKey &key, Cacher::value_type &out) {
	auto handle = hash_map.find(key);
	if(handle == hash_map.end()){
		return false;
	}
	out = handle->second;
	return true;
}
bool Cacher::lookupTrees(const PllSplit &spl_a, const PllSplit &spl_b, Cacher::value_type &out) {
	auto key = TwoSplitHashmapKey(&spl_a,&spl_b);
	return lookupTrees(key, out);
}
void Cacher::putTrees(const PllSplit &spl_a, const PllSplit &spl_b, value_type val) {
    auto key = TwoSplitHashmapKey(&spl_a,&spl_b);
    /*auto handle = */hash_map.insert(std::make_pair(key, val));
}
Cacher::Cacher() : hash_map(initial_size) {
}
