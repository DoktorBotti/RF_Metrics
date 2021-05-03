
#ifndef INFORF_RFALGORITHMINTERFACE_H
#define INFORF_RFALGORITHMINTERFACE_H
#include "PllTree.hpp"
#include "SymmetricMatrix.hpp"
#include <vector>

class RfAlgorithmInterface {
  public:
    virtual SymmetricMatrix<double> calculate(std::vector<PllTree> &trees);
};
#endif // INFORF_RFALGORITHMINTERFACE_H
