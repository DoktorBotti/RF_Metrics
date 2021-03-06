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
#include <future>

class GeneralizedRfAlgo : public RfAlgorithmInterface {
  public:
	GeneralizedRfAlgo();
	explicit GeneralizedRfAlgo(size_t split_len);
	GeneralizedRfAlgo(GeneralizedRfAlgo &&) = default;
	~GeneralizedRfAlgo() override = default;
	RfMetricInterface::Results calculate(std::vector<PllTree> &trees,
	                                     const RfMetricInterface::Params &params) override;
	// precomputes the logarithm and factorials
	static LogDblFact factorials;

    // calculates the metric score between two splits
	// TODO: make protected
    virtual Scalar
    calc_split_score(const PllSplit &S1, const PllSplit &S2) = 0;
    RfAlgorithmInterface::Scalar calc_tree_score(const SplitList &A, const SplitList &B);

  protected:
	/* Calculates the information content of a split with partition sizes a and b. */
	virtual Scalar h_info_content(size_t a, size_t b);
	[[maybe_unused]] SplitScores calc_pairwise_split_scores(const SplitList &S1, const SplitList &S2);
	// method to use if both pll-splits are equal. Often more simple solution possible
	virtual Scalar calc_split_score(const PllSplit &S1) = 0;

	virtual RfAlgorithmInterface::Scalar calc_tree_info_content(const SplitList &S);
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
	void compute_split_comparison(const PllSplit &S1, const PllSplit &S2);
	// Stores intermediate results such as intersections. For more information see
	// compute_split_comparison
	std::vector<PllSplit> temporary_splits;
	SymmetricMatrix<Scalar> pairwise_split_scores;
	size_t taxa = 0;

  private:
    MatcherOrTools match_solver;
    const size_t max_parallel_threads = 600;
    std::vector<PllSplit> unique_pll_splits;
    std::vector<pll_split_base_t> temporary_split_content; // DANGER! Not to be operated by fuckwits
    static std::mutex pairwise_tree_score_mutex;
    static std::atomic_size_t tree_idx;

    // logging stuff
    boost::log::sources::severity_logger<lg::SeverityLevel> logger;
	size_t stat_calculated_trees = 0;

	// private functions to precompute splits and scores
	void setup_temporary_storage(size_t split_len);
	std::vector<FastSplitList> generateFastList(const std::vector<PllSplitList> &active_slow_list);
	SymmetricMatrix<Scalar> calcPairwiseSplitScores();

	// parallelization functions which compute max_parallel_threads many pairwise tree scores in parallel
	static void calc_thread(GeneralizedRfAlgo &alg, const std::vector<FastSplitList> &trees,
	                 size_t pairwise_tree_cnt,
	                 SymmetricMatrix<Scalar> &sim);
	//static void execute_bla(GeneralizedRfAlgo &alg )
};

#endif // INFORF_GENERALIZEDRFALGO_H
