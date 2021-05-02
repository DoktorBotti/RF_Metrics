
#ifndef INFORF_RFALGORITHMINTERFACE_H
#define INFORF_RFALGORITHMINTERFACE_H
#include "PllTree.hpp"
#include <vector>
#include "SymmetricMatrix.h"

class RfAlgorithmInterface
{
  public:
     SymmetricMatrix calculate(const std::vector<PllTree>& trees);
};
#endif // INFORF_RFALGORITHMINTERFACE_H
