#include "../libs/growt/allocator/alignedallocator.hpp"
#include "../libs/growt/data-structures/hash_table_mods.hpp"
#include "../libs/growt/data-structures/table_config.hpp"
#include "../libs/growt/utils/default_hash.hpp"

#include <cassert>
#include <cstdio>
#include <vector>
class TwoSplitHashmapKey {
  public:
	// These functions are by growt to generate a hash
	[[nodiscard]] const void *data() const{
	    return &pre_hash[0];
	};
	[[nodiscard]] int size() const{
	    return sizeof(unsigned int) * 5;
	};

  private:
	// pointers to actual splits
	unsigned int const *split_a;
	unsigned int const *split_b;

	// pre-hashing for our symmetry purposes
	// this is just the splits a and b xored together. We maybe want to reduce this to a constant
	// size.
	std::vector<unsigned int> pre_hash;

  public:
	// constructors
	TwoSplitHashmapKey();
	TwoSplitHashmapKey(const TwoSplitHashmapKey &oth) = default;
	TwoSplitHashmapKey(TwoSplitHashmapKey &&oth) = default;
	explicit TwoSplitHashmapKey(unsigned int const *dataA, unsigned int const *dataB)
	    : split_a(dataA), split_b(dataB){
	}
	~TwoSplitHashmapKey() = default;
	// either a must match a or a must match b
	bool operator==(const TwoSplitHashmapKey &rhs) const;
	bool operator!=(const TwoSplitHashmapKey &rhs) const;
};

class Cacher {
	using value_type = double;
	using key_type = TwoSplitHashmapKey;
	using allocator_type = growt::AlignedAllocator<>;
	using table_conf_instance = growt::table_config<key_type,
	                                                value_type,
	                                                utils_tm::hash_tm::default_hash,
	                                                allocator_type,
	                                                hmod::growable>;
	using table_type = typename table_conf_instance::table_type;
	table_type hash_map;

	static const size_t initial_size = 200;

  public:
	bool lookupTrees(TwoSplitHashmapKey &key, value_type &out) {
		auto handle = hash_map.get_handle();
		auto found = handle.find(key);
		if (found == handle.end()) {
			return false;
		}
		out = found->second;
		return true;
	};
	bool lookupTrees(unsigned int const *spl_a, unsigned int const *spl_b, value_type &out) {
		auto key = TwoSplitHashmapKey(spl_a, spl_b);
		return lookupTrees(key, out);
	};
	void putTrees(unsigned int const *spl_a, unsigned int const *spl_b, value_type val) {
		auto key = TwoSplitHashmapKey(spl_a, spl_b);
		auto handle = hash_map.get_handle();
		handle.insert(key, val);
	};
	Cacher() : hash_map(1){};
};
int main() {
	Cacher cache;
	// all these uint pointers are in reality more sensible things ;)
	unsigned int const * data_a = new unsigned int[5];
	unsigned int const * data_b = new unsigned int[5];
	cache.putTrees(data_a,data_b, 42.);

	double foundVal = 0.;
	bool success = cache.lookupTrees(data_a, data_b, foundVal);
	assert(success);
	assert(foundVal == 42.);
	return 0;
}