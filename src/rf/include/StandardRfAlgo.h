//
// Created by Tobias Bodmer on 02.05.2021.
//

#ifndef INFORF_STANDARDRFALGO_H
#define INFORF_STANDARDRFALGO_H
#include "HashingFunctor.hpp"
#include "PllTree.hpp"
#include "RfAlgorithmInterface.h"
#include <boost/log/sources/severity_logger.hpp>

#include <vector>

class StandardRfAlgo : public RfAlgorithmInterface {
  public:
	StandardRfAlgo();
	StandardRfAlgo(const StandardRfAlgo &) = default;
	StandardRfAlgo(StandardRfAlgo &&) = default;
	~StandardRfAlgo() override = default;
	RfMetricInterface::Results calculate(std::vector<PllTree> &trees) override;

  private:
	boost::log::sources::severity_logger<lg::SeverityLevel> logger;
};

#endif // INFORF_STANDARDRFALGO_H
