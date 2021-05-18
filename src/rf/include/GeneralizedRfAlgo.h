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
    virtual double calc_tree_score(const PllSplitList& A, const PllSplitList& B ) = 0;
	double phylogenetic_prob(const PllSplit& split_a, const PllSplit& split_b);
	double phylogenetic_prob(const PllSplit& split);
	virtual SymmetricMatrix<double> calc_pairwise_split_scores(const PllSplitList& S1, const PllSplitList& S2) = 0;
  private:
    boost::log::sources::severity_logger<lg::SeverityLevel> logger;
};


#endif // INFORF_GENERALIZEDRFALGO_H