
#ifndef INFORF_RFALGORITHMINTERFACE_H
#define INFORF_RFALGORITHMINTERFACE_H
#include "PllTree.hpp"
#include "RfMetricInterface.h"
#include <vector>
class RfAlgorithmInterface {
  public:
	typedef double Scalar;
	virtual RfMetricInterface::Results calculate(std::vector<PllTree> &trees) = 0;
	// avoid v-table issues by defining trivial destructor in its own compile unit
	virtual ~RfAlgorithmInterface();
};


#endif // INFORF_RFALGORITHMINTERFACE_H
