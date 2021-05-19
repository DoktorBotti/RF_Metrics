//
// Created by Tobia on 16.05.2021.
//

#ifndef INFORF_MSIALGO_H
#define INFORF_MSIALGO_H
#include "GeneralizedRfAlgo.h"
#include "PllSplits.hpp"
class MsiAlgo : public GeneralizedRfAlgo {
  protected:
	double calc_tree_score(const PllSplitList &A, const PllSplitList &B) override;
};

#endif // INFORF_MSIALGO_H
