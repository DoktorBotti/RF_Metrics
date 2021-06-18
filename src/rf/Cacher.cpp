#include "Cacher/Cacher.h"
bool Cacher::lookupTrees(CacherKey &key, Cacher::value_type &out) {
	auto handle = hash_map.get_handle();
	auto found = handle.find(key);
	if(found == handle.end()){
		return false;
	}
	out = found->second;
	return true;
}
bool Cacher::lookupTrees(const PllSplit *spl_a, const PllSplit *spl_b, Cacher::value_type &out) {
	auto key = CacherKey(spl_a,spl_b);
	return lookupTrees(key, out);
}
void Cacher::putTrees(const PllSplit *spl_a, const PllSplit *spl_b, value_type val) {
    auto key = CacherKey(spl_a,spl_b);
    auto handle = hash_map.get_handle();
	handle.insert(key, val);
}
Cacher::Cacher() : hash_map(initial_size) {
}
