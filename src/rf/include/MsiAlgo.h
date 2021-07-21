#ifndef INFORF_MSIALGO_H
#define INFORF_MSIALGO_H
#include "GeneralizedRfAlgo.h"
#include "PllSplit.hpp"
class MsiAlgo : public GeneralizedRfAlgo {
  protected:
  public:
	explicit MsiAlgo(size_t split_len);
	MsiAlgo() = default;
	MsiAlgo(MsiAlgo &&) = default;
	~MsiAlgo() override = default;
	Scalar calc_split_score(const PllSplit &S1, const PllSplit &S2) override;
	Scalar calc_split_score(const PllSplit &S1) override;
};

#endif // INFORF_MSIALGO_H
