//
// Created by Robert Schmoltzi on 18.05.21.
//

#ifndef CONCURRENT_GROWTABLE_MCIALGO_H
#define CONCURRENT_GROWTABLE_MCIALGO_H

#include "GeneralizedRfAlgo.h"

class MciAlgo : public GeneralizedRfAlgo {
  protected:
	SymmetricMatrix<double> calc_pairwise_split_scores(const PllSplitList &S1,
	                                                   const PllSplitList &S2) override;

  public:
	// TODO: make private and still testable?
    static double info_cl(PllSplit split1, PllSplit split2, size_t taxa, size_t split_len);
};

#endif // CONCURRENT_GROWTABLE_MCIALGO_H
