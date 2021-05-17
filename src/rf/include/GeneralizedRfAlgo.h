//
// Created by Tobia on 16.05.2021.
//

#ifndef INFORF_GENERALIZEDRFALGO_H
#define INFORF_GENERALIZEDRFALGO_H
#include "RfAlgorithmInterface.h"
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/dynamic_bitset.hpp>


class GeneralizedRfAlgo : public RfAlgorithmInterface {
  public:
    GeneralizedRfAlgo();
    GeneralizedRfAlgo(const GeneralizedRfAlgo &) = default;
    GeneralizedRfAlgo(GeneralizedRfAlgo &&) = default;
    ~GeneralizedRfAlgo() override = default;
    RfMetricInterface::Results calculate(std::vector<PllTree> &trees) override;

  protected:
    virtual double calcScore(const PllSplitList& A, const PllSplitList& B ) = 0;
	double phylogeneticProbability(const PllSplit& split_a, const PllSplit& split_b);
	double phylogeneticProbability(const PllSplit& split);

  private:
    boost::log::sources::severity_logger<lg::SeverityLevel> logger;
};


#endif // INFORF_GENERALIZEDRFALGO_H
