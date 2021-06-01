//
// Created by Tobia on 16.05.2021.
//

#ifndef INFORF_GENERALIZEDRFALGO_H
#define INFORF_GENERALIZEDRFALGO_H
#include "MinFlowMatcher.h"
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
	virtual double calc_tree_score(const PllSplitList &A, const PllSplitList &B);
	static double p_phy(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len);
	static double p_phy(const PllSplit &S, size_t taxa, size_t split_len);
	/* Calculates the phylogenetic probability of a split with partition sizes a and b. */
	static double p_phy(size_t a, size_t b);
	static double h_info_content(const PllSplit &S, size_t taxa, size_t split_len);
	static double
	h_info_content(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len);
	/* Calculates the information content of a split with partition sizes a and b. */
	static double h_info_content(size_t a, size_t b);
	SymmetricMatrix<double> calc_pairwise_split_scores(const PllSplitList &S1,
	                                                   const PllSplitList &S2);
	virtual double
	calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) = 0;
	static size_t bits_too_many(size_t taxa);
	/*
	 * Returns a vec with 6 blocks of split_len size.
	 * It contains:
	 * split_buffer[0] : B1
	 * split_buffer[split_len] : B2
	 * split_buffer[2 * split_len] : A1 and A2
	 * split_buffer[3 * split_len] : B1 and B2
	 * split_buffer[4 * split_len] : A1 and B2
	 * split_buffer[5 * split_len] : A2 and B1
	 */
	static std::vector<pll_split_base_t>
	compute_split_comparison(const PllSplit &S1, const PllSplit &S2, size_t split_len);

  private:
    boost::log::sources::severity_logger<lg::SeverityLevel> logger;
    MinFlowMatcher match_solver;
};

#endif // INFORF_GENERALIZEDRFALGO_H
