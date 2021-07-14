#ifndef INFORF_FASTSPLITLIST_H
#define INFORF_FASTSPLITLIST_H
#include "PllSplitList.h"
#include "SplitList.h"

class FastSplitList : public SplitList {
  public:
	explicit FastSplitList(size_t num_taxa);
	~FastSplitList() override = default;
	PllSplit &operator[](size_t index) override;
	const PllSplit &operator[](size_t index) const override;
	[[nodiscard]] size_t size() const override;
	static void setBasePtr(PllSplit *ptr);

	void setOffsetAt(unsigned long change_offset, size_t newValue);

  private:
	// we store the index to all PllSplits as offsets from a (static) base address
	std::vector<size_t> split_idx;
	static PllSplit *base_ptr;
};

#endif // INFORF_FASTSPLITLIST_H
