#ifndef RF_METRICS_CACHER_H
#define RF_METRICS_CACHER_H
#include <unordered_map>

#include "Matcher.h"
#include "TwoSplitHashmapKey.h"
#include "TwoSplitHashmapFunctor.h"


class Cacher {
	using value_type = Matcher::Scalar;
    std::unordered_map<TwoSplitHashmapKey, value_type, TwoSplitHashmapFunctor> hash_map;
	static const size_t initial_size = 200;

  public:
	bool lookupTrees(TwoSplitHashmapKey & key, value_type& out);
	bool lookupTrees(PllSplit const& spl_a, PllSplit const& spl_b, value_type& out);
	void putTrees(PllSplit const& spl_a, PllSplit const& spl_b, value_type val);
	Cacher();
};

#endif // RF_METRICS_CACHER_H
