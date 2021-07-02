#ifndef INFORF_GENERALIZEDRFALGO_H
#define INFORF_GENERALIZEDRFALGO_H
#include "FastSplitList.h"
#include "LogDblFact.h"
#include "MatcherOrTools.h"
#include "RectMatrix.hpp"
#include "RfAlgorithmInterface.h"
#include <boost/dynamic_bitset.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/severity_logger.hpp>

class GeneralizedRfAlgo : public RfAlgorithmInterface {
  public:
	GeneralizedRfAlgo();
	explicit GeneralizedRfAlgo(size_t split_len);
	GeneralizedRfAlgo(GeneralizedRfAlgo &&) = default;
	~GeneralizedRfAlgo() override = default;
	RfMetricInterface::Results calculate(std::vector<PllTree> &trees) override;
	// should be protected but testing... (too lazy)
	static LogDblFact factorials;

  protected:
	virtual Scalar calc_tree_score(const SplitList &A, const SplitList &B);
	static Scalar p_phy(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len);
	static Scalar p_phy(const PllSplit &S, size_t taxa, size_t split_len);
	/* Calculates the phylogenetic probability of a split with partition sizes a and b. */
	static Scalar p_phy(size_t a, size_t b);
	static Scalar h_info_content(const PllSplit &S, size_t taxa, size_t split_len);
	static Scalar
	h_info_content(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len);
	/* Calculates the information content of a split with partition sizes a and b. */
	static Scalar h_info_content(size_t a, size_t b);
	SplitScores calc_pairwise_split_scores(const SplitList &S1, const SplitList &S2);

  public:
	virtual Scalar
	calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) = 0;
	// method to use if both pll-splits are equal. Often more simple solution possible
    virtual Scalar calc_split_score(const PllSplit &S1, size_t taxa, size_t split_len) = 0;

  protected:
	virtual Scalar calc_tree_info_content(const SplitList &S, size_t taxa, size_t split_len);
	void calc_pairwise_tree_dist(const std::vector<FastSplitList> &trees,
	                             RfMetricInterface::Results &res);
	static size_t bits_too_many(size_t taxa);
	/*
	 * Calculates PllSplits inside temporary_splits
	 * It contains:
	 * temporary_splits[0] : B1
	 * temporary_splits[1] : B2
	 * temporary_splits[2] : A1 and A2
	 * temporary_splits[3] : B1 and B2
	 * temporary_splits[4] : A1 and B2
	 * temporary_splits[5] : A2 and B1
	 */
	virtual void compute_split_comparison(const PllSplit &S1, const PllSplit &S2, size_t split_len);
	// Stores intermediate results such as intersections. For more information see
	// compute_split_comparison
	std::vector<PllSplit> temporary_splits;
	std::vector<pll_split_base_t> temporary_split_content; // DANGER! Not to be operated by fuckwits

  private:
	boost::log::sources::severity_logger<lg::SeverityLevel> logger;
	std::vector<PllSplit> unique_pll_splits;
	MatcherOrTools match_solver;
	void setup_temporary_storage(size_t split_len);
	std::vector<FastSplitList> generateFastList(const std::vector<PllSplitList> &active_slow_list);
};

#endif // INFORF_GENERALIZEDRFALGO_H
