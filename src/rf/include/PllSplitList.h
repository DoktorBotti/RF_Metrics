#ifndef INFORF_PLLSPLITLIST_H
#define INFORF_PLLSPLITLIST_H
#include "SplitList.h"
#include <vector>

class PllTree;

class PllSplitList : public SplitList {
  public:
    explicit PllSplitList(const PllTree &tree);

    /* Rule of 5 constructors/destructors */
    virtual ~PllSplitList();

    PllSplitList(const PllSplitList &other);

    PllSplitList(PllSplitList &&other) noexcept : _splits(std::exchange(other._splits, {})), _tree_id(other._tree_id) {
    }

    PllSplitList &operator=(const PllSplitList &other) {
        *this = PllSplitList(other);
        return *this;
    }

    PllSplitList &operator=(PllSplitList &&other) noexcept {
        std::swap(_splits, other._splits);
        return *this;
    }

    PllSplit operator[](size_t index) const override {
        return _splits[index];
    }

    [[nodiscard]] size_t size() const override {
        return _splits.size();
    }


    [[nodiscard]] PllSplit const *getPtrToNthElem(size_t i) const {
        return &_splits.at(i);
    }

    [[nodiscard]] size_t inline getTreeId() const {
        return _tree_id;
    }
    void inline setTreeId(size_t id) {
        _tree_id = id;
    }

  private:

    std::vector<PllSplit> _splits;
    size_t _tree_id = 0;
};

#endif // INFORF_PLLSPLITLIST_H
