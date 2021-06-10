//
// Created by Robert Schmoltzi on 19.05.21.
//

#ifndef CONCURRENT_GROWTABLE_SPIALGO_H
#define CONCURRENT_GROWTABLE_SPIALGO_H

#include "GeneralizedRfAlgo.h"
class SpiAlgo : public GeneralizedRfAlgo {
  public:
	/* Calculates whether the two splits are compatible. */
    static bool compatible(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len);
    double calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) override;

  protected:
};

#endif // CONCURRENT_GROWTABLE_SPIALGO_H
