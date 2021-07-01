#pragma once
#include "PllSplit.hpp"

struct OneSplitHashmapKey {
  public:
	PllSplit const *split;
	explicit OneSplitHashmapKey(PllSplit const *copySplit) : split(copySplit) {
	}
	OneSplitHashmapKey(const OneSplitHashmapKey &oth) = default;
	OneSplitHashmapKey(OneSplitHashmapKey &&oth) = default;
	~OneSplitHashmapKey() = default;
	bool operator==(const OneSplitHashmapKey &rhs) const {
		for (size_t i = 0; i < PllSplit::split_len; ++i) {
			if (split->operator()()[i] != rhs.split->operator()()[i]) {
				return false;
			}
		}
		return true;
	}
	bool operator!=(const OneSplitHashmapKey &rhs) const {
		return !(rhs == *this);
	}
};
