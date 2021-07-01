#ifndef RF_METRICS_TWOSPLITHASHMAPKEY_H
#define RF_METRICS_TWOSPLITHASHMAPKEY_H

#include <PllSplit.hpp>
class TwoSplitHashmapKey {
  public:
	// constructors
	TwoSplitHashmapKey();
	TwoSplitHashmapKey(const TwoSplitHashmapKey &oth) = default;
	TwoSplitHashmapKey(TwoSplitHashmapKey &&oth) = default;
	explicit TwoSplitHashmapKey(PllSplit const *copy_split_a, PllSplit const *copy_split_b);
	~TwoSplitHashmapKey() = default;

	// actual splits, can be public since they are const anyway
	PllSplit split_a;
	PllSplit split_b;
	size_t pre_hash = 0;

  public:
	// either a must match a or a must match b
	bool operator==(const TwoSplitHashmapKey &rhs) const;
	bool operator!=(const TwoSplitHashmapKey &rhs) const;
};

#endif // RF_METRICS_TWOSPLITHASHMAPKEY_H
