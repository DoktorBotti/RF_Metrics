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
    double calc_split_score( PllSplit &S1,  PllSplit &S2, size_t taxa, size_t split_len) override;
};

#endif // INFORF_MSIALGO_H
