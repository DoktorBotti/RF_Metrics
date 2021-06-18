#include "include/Cacher/CacherKey.h"
bool CacherKey::operator==(const CacherKey &rhs) const {
	bool a_a = split_a()[0] == rhs.split_a()[0];
	bool b_b = split_b()[0] == rhs.split_b()[0];
	bool b_a = split_b()[0] == rhs.split_a()[0];
	bool a_b = split_a()[0] == rhs.split_b()[0];
	bool a_matches_a = a_a && b_b; // and implicitly b_matches_b
	bool a_matches_b = b_a && a_b; // and implicitly b_matches_a

	if (!a_matches_a && !a_matches_b) {
		// in the first segment, no similarity is found
		return false;
	}
	for (size_t i = 1; i < PllSplit::split_len; ++i) {
		assert(a_matches_a || a_matches_b);
		// when a single segment does not match, the other combination might still be valid
		// only compare if needed (branching might be worth it)
		if (a_matches_a) {
			a_matches_a = split_a()[i] == rhs.split_a()[i] && split_b()[i] == rhs.split_b()[i];
		}
		if (a_matches_b) {
			a_matches_b = split_b()[i] == rhs.split_a()[i] && split_a()[i] == rhs.split_b()[i];
		}
		if (!a_matches_a && !a_matches_b) {
			return false;
		}
	}
	return true;
}
bool CacherKey::operator!=(const CacherKey &rhs) const {
	return !(rhs == *this);
}
CacherKey::CacherKey() : split_a(nullptr), split_b(nullptr) {
}
int CacherKey::size() const {
	return static_cast<int>(sizeof(split_b()[0])) * PllSplit::split_len;
}
const void *CacherKey::data() const {
	// need to xor all split a_s and b_s to range somewhere in storage and pass the pointer to it.
	return static_cast<const void *>(&pre_hash[0]);
}
CacherKey::CacherKey(const PllSplit *copy_split_a, const PllSplit *copy_split_b)
    : split_a(*copy_split_a), split_b(*copy_split_b), pre_hash(PllSplit::split_len) {
	for (size_t i = 0; i < PllSplit::split_len; ++i) {
		pre_hash[i] = split_b()[i] ^ split_a()[i];
	}
}
