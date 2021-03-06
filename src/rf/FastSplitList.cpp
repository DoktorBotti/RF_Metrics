#include "FastSplitList.h"
PllSplit *FastSplitList::base_ptr = nullptr;

size_t FastSplitList::size() const {
	return split_idx.size();
}
FastSplitList::FastSplitList(size_t num_taxa) : split_idx(num_taxa) {
}
PllSplit &FastSplitList::operator[](size_t index) {
	assert(index < size());
	assert(base_ptr);
	return base_ptr[split_idx[index]];
}
const PllSplit &FastSplitList::operator[](size_t index) const {
	assert(index < size());
	assert(base_ptr);
	return base_ptr[split_idx[index]];
}
void FastSplitList::setBasePtr(PllSplit *ptr) {
    base_ptr = ptr;
}
void FastSplitList::setOffsetAt(unsigned long change_offset, size_t newValue) {
	assert(change_offset < split_idx.size());
	split_idx[change_offset] = newValue;
}
