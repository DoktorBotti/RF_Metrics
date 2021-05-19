//
// Created by Tobia on 16.05.2021.
//

#ifndef INFORF_MSIALGO_H
#define INFORF_MSIALGO_H
#include "GeneralizedRfAlgo.h"
#include "PllSplits.hpp"
class MsiAlgo : public GeneralizedRfAlgo {
  protected:
	SymmetricMatrix<double> calc_pairwise_split_scores(const PllSplitList &S1,
	                                                   const PllSplitList &S2) override;
};

#endif // INFORF_MSIALGO_H
