#ifndef RF_METRICS_CACHER_H
#define RF_METRICS_CACHER_H

#include "CacherKey.h"
//#include "tbb/scalable_allocator.h"
#include "data-structures/table_config.hpp"
#include "allocator/poolallocator.hpp"

class Cacher {
	using value_type = Matcher::Scalar;
	using key_type = CacherKey;
	using allocator_type = growt::PoolAllocator<>;
	using table_conf_instance = growt::table_config<key_type,
	                                                value_type,
	                                                utils_tm::hash_tm::default_hash,
	                                                allocator_type,
	                                                hmod::growable>;
	using table_type = typename table_conf_instance::table_type;
	table_type hash_map;

	static const size_t initial_size = 200;

  public:
	bool lookupTrees(CacherKey& key, value_type& out);
	bool lookupTrees(PllSplit const* spl_a, PllSplit const* spl_b, value_type& out);
	void putTrees(PllSplit const* spl_a, PllSplit const* spl_b, value_type val);
	Cacher();
};

#endif // RF_METRICS_CACHER_H
