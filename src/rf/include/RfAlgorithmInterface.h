
#ifndef INFORF_RFALGORITHMINTERFACE_H
#define INFORF_RFALGORITHMINTERFACE_H
#include "PllTree.hpp"
#include "SymmetricMatrix.h"
#include <vector>

class RfAlgorithmInterface {
  public:
    virtual SymmetricMatrix calculate(std::vector<PllTree> &trees);
};
#endif // INFORF_RFALGORITHMINTERFACE_H
