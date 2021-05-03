//
// Created by Tobias Bodmer on 02.05.2021.
//

#ifndef INFORF_STANDARDRFALGO_H
#define INFORF_STANDARDRFALGO_H
#include "RfAlgorithmInterface.h"

class StandardRfAlgo : public RfAlgorithmInterface {
  public:
    SymmetricMatrix<double> calculate(std::vector<PllTree> &trees) override;
};

#endif // INFORF_STANDARDRFALGO_H
