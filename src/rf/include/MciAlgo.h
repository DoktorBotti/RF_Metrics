//
// Created by Robert Schmoltzi on 18.05.21.
//

#ifndef CONCURRENT_GROWTABLE_MCIALGO_H
#define CONCURRENT_GROWTABLE_MCIALGO_H

#include "GeneralizedRfAlgo.h"

class MciAlgo : public GeneralizedRfAlgo {
  protected:

  public:
	// TODO: make private and still testable?
    double calc_split_score(const PllSplit &split1, const PllSplit &split2, size_t taxa, size_t split_len) override;
};

#endif // CONCURRENT_GROWTABLE_MCIALGO_H
