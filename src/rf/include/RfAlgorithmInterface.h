
#ifndef INFORF_RFALGORITHMINTERFACE_H
#define INFORF_RFALGORITHMINTERFACE_H
#include "PllTree.hpp"
#include "RfMetricInterface.h"
#include <vector>
class RfAlgorithmInterface {
  public:
	virtual RfMetricInterface::Results calculate(std::vector<PllTree> & trees) = 0;
	virtual ~RfAlgorithmInterface() = default;
};

#endif // INFORF_RFALGORITHMINTERFACE_H
