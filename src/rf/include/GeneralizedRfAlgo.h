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
    static double p_phy(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len);
    static double p_phy(const PllSplit &S, size_t taxa, size_t split_len);
    static double h_info_content(const PllSplit &S, size_t taxa, size_t split_len);
    static double h_info_content(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len);
	SymmetricMatrix<double> calc_pairwise_split_scores(const PllSplitList& S1, const PllSplitList& S2);
    virtual double calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) = 0;
  private:
    boost::log::sources::severity_logger<lg::SeverityLevel> logger;
	Matcher match_solver;
};


#endif // INFORF_GENERALIZEDRFALGO_H
