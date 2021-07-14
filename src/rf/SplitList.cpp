#include "SplitList.h"
size_t SplitList::computeSplitLen() const {
    size_t tip_count = size() + 3;
    size_t split_len = (tip_count / computeSplitBaseSize());

    if ((tip_count % computeSplitBaseSize()) > 0) {
        split_len += 1;
    }

    return split_len;
}
constexpr size_t SplitList::computeSplitBaseSize() {
    return sizeof(pll_split_base_t) * 8;
}
size_t SplitList::computeSplitArraySize() const {
    return computeSplitLen() * size();
}
SplitList::~SplitList() = default;
