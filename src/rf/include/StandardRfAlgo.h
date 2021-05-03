//
// Created by Tobias Bodmer on 02.05.2021.
//

#ifndef INFORF_STANDARDRFALGO_H
#define INFORF_STANDARDRFALGO_H
#include "RfAlgorithmInterface.h"
#include "PllTree.hpp"
#include <vector>
class StandardRfAlgo : public RfAlgorithmInterface {
  public:
	RfMetricInterface::Results calculate(std::vector<PllTree> &trees) override;
};

#endif // INFORF_STANDARDRFALGO_H
