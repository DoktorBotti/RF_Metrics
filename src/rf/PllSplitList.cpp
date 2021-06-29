#include "PllSplitList.h"
#include "PllTree.hpp"

PllSplitList::PllSplitList(const PllTree &tree) {
	auto tmp_splits =
	    pllmod_utree_split_create(tree.tree()->vroot, tree.tree()->tip_count, nullptr);

	for (size_t i = 0; i < tree.tree()->tip_count - 3; ++i) {
		_splits.emplace_back(tmp_splits[i]);
	}
    delete_ptr = _splits[0]();
	free(tmp_splits);
}

PllSplitList::PllSplitList(const PllSplitList &other) {
	auto tmp_splits =
	    static_cast<pll_split_t>(calloc(other.computeSplitArraySize(), sizeof(pll_split_base_t)));
	_tree_id = other._tree_id;
	memcpy(
	    tmp_splits, other._splits[0](), other.computeSplitArraySize() * sizeof(pll_split_base_t));

	for (size_t i = 0; i < other.computeSplitArraySize(); ++i) {
		_splits.emplace_back(tmp_splits + (i * other.computeSplitLen()));
	}
    delete_ptr = _splits[0]();

}

PllSplitList::~PllSplitList() {
	if (!_splits.empty()) {
		free(delete_ptr); // Probably fine. Always allocate all splits in a single chunk of
		                    // memory!!
	}
}
PllSplitList &PllSplitList::operator=(const PllSplitList &other) {
	*this = PllSplitList(other);
	return *this;
}
PllSplitList &PllSplitList::operator=(PllSplitList &&other) noexcept {
	std::swap(_splits, other._splits);
	return *this;
}
const PllSplit &PllSplitList::operator[](size_t index) const {
	return _splits[index];
}
PllSplit &PllSplitList::operator[](size_t index) {
	return _splits[index];
}
PllSplit const *PllSplitList::getPtrToNthElem(size_t i) const {
	return &_splits.at(i);
}
size_t PllSplitList::getTreeId() const {
	return _tree_id;
}
void PllSplitList::setTreeId(size_t id) {
	_tree_id = id;
}
size_t PllSplitList::size() const {
	return _splits.size();
}
PllSplitList::PllSplitList(PllSplitList &&other) noexcept
    : _splits(std::exchange(other._splits, {})), _tree_id(other._tree_id) {
}
auto PllSplitList::begin() -> decltype(std::vector<PllSplit>::iterator()) {
	return _splits.begin();
}
auto PllSplitList::end() -> decltype(std::vector<PllSplit>::iterator()) {
	return _splits.end();
}
