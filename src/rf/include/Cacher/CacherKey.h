#ifndef RF_METRICS_CACHERKEY_H
#define RF_METRICS_CACHERKEY_H

#include "Matcher.h"
#include <PllSplits.hpp>
class CacherKey {
  public:
	// These functions are used to generate the hash
	[[nodiscard]] const void *data() const;
	[[nodiscard]] int size() const;

	// constructors
	CacherKey();
	CacherKey(const CacherKey &oth) = default;
	CacherKey(CacherKey &&oth) = default;
	explicit CacherKey(PllSplit const *copy_split_a, PllSplit const *copy_split_b);
	~CacherKey() = default;

  private:
	// pointers to actual splits
	PllSplit const split_a;
    PllSplit const split_b;

    // pre-hashing for our symmetry purposes
    // this is just the splits a and b xored together. We maybe want to reduce this to a constant size.
    // TODO: check if constant storage is all in all faster.
    std::vector<std::remove_pointer<decltype(split_b())>::type> pre_hash;

  public:
	// either a must match a or a must match b
	bool operator==(const CacherKey &rhs) const;
	bool operator!=(const CacherKey &rhs) const;
};

#endif // RF_METRICS_CACHERKEY_H
