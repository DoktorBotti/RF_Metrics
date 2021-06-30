//
// Created by Tobia on 16.05.2021.
//

#ifndef INFORF_MSIALGO_H
#define INFORF_MSIALGO_H
#include "GeneralizedRfAlgo.h"
#include "PllSplits.hpp"
class MsiAlgo : public GeneralizedRfAlgo {
  protected:
  public:
	Scalar calc_split_score(const PllSplit &S1,
	                        const PllSplit &S2,
	                        size_t taxa,
	                        size_t split_len) override;
};

#endif // INFORF_MSIALGO_H
