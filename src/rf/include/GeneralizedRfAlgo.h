//
// Created by Tobia on 16.05.2021.
//

#ifndef INFORF_GENERALIZEDRFALGO_H
#define INFORF_GENERALIZEDRFALGO_H
#include "Matcher.h"
#include "RfAlgorithmInterface.h"
#include <boost/dynamic_bitset.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/severity_logger.hpp>

class GeneralizedRfAlgo : public RfAlgorithmInterface {
  public:
    GeneralizedRfAlgo();
    GeneralizedRfAlgo(GeneralizedRfAlgo &&) = default;
    ~GeneralizedRfAlgo() override = default;
    RfMetricInterface::Results calculate(std::vector<PllTree> &trees) override;
  protected:
    virtual double calc_tree_score(const PllSplitList& A, const PllSplitList& B );
    double phylogenetic_prob(const PllSplit& split_a, const PllSplit& split_b);
	double phylogenetic_prob(const PllSplit& split);
	virtual SymmetricMatrix<double> calc_pairwise_split_scores(const PllSplitList& S1, const PllSplitList& S2) = 0;
  private:
    boost::log::sources::severity_logger<lg::SeverityLevel> logger;
	Matcher match_solver;
};


#endif // INFORF_GENERALIZEDRFALGO_H
