#ifndef INFORF_PLLSPLITLIST_H
#define INFORF_PLLSPLITLIST_H
#include "SplitList.h"
#include <vector>

class PllTree;

class PllSplitList : public SplitList {
  public:
	/* Rule of 5 constructors/destructors */
	virtual ~PllSplitList();
	explicit PllSplitList(const PllTree &tree);
	PllSplitList(const PllSplitList &other);
	PllSplitList(PllSplitList &&other) noexcept;

	PllSplitList &operator=(const PllSplitList &other);
	PllSplitList &operator=(PllSplitList &&other) noexcept;
	/* access and size operators */
	const PllSplit &operator[](size_t index) const override;
	PllSplit &operator[](size_t index) override;
	[[nodiscard]] size_t size() const override;

	[[nodiscard]] PllSplit const *getPtrToNthElem(size_t i) const;
	[[nodiscard]] size_t getTreeId() const;
	void setTreeId(size_t id);

  private:
	std::vector<PllSplit> _splits;
	size_t _tree_id = 0;
};

#endif // INFORF_PLLSPLITLIST_H
