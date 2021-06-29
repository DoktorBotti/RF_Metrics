#pragma once
#include "PllSplit.hpp"

struct HashmapKey {
  public:
	PllSplit const *split;
	explicit HashmapKey(PllSplit const *copySplit) : split(copySplit) {
	}
	HashmapKey(const HashmapKey &oth) = default;
	HashmapKey(HashmapKey &&oth) = default;
	~HashmapKey() = default;
	bool operator==(const HashmapKey &rhs) const {
		for (size_t i = 0; i < PllSplit::split_len; ++i) {
			if (split->operator()()[i] != rhs.split->operator()()[i]) {
				return false;
			}
		}
		return true;
	}
	bool operator!=(const HashmapKey &rhs) const {
		return !(rhs == *this);
	}
};
