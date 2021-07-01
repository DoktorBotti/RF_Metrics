//
// Created by Robert Schmoltzi on 18.05.21.
//

#ifndef CONCURRENT_GROWTABLE_MCIALGO_H
#define CONCURRENT_GROWTABLE_MCIALGO_H

#include "GeneralizedRfAlgo.h"

class MciAlgo : public GeneralizedRfAlgo {
  private:
	static inline Scalar
	to_prob(size_t numerator_inout_log, size_t numerator_in_log, size_t denom_a, size_t denom_b);

  public:
	// TODO: make private and still testable?
	explicit MciAlgo(size_t split_len);
	MciAlgo() = default;
	MciAlgo(MciAlgo&&) = default;
	~MciAlgo() override = default;

	Scalar calc_split_score(const PllSplit &S1,
	                        const PllSplit &S2,
	                        size_t taxa,
	                        size_t split_len) override;
	RfAlgorithmInterface::Scalar
	calc_tree_info_content(const PllSplitList &S, size_t taxa, size_t split_len) override;
	Scalar calc_split_score(const PllSplit &S1, size_t taxa, size_t split_len) override;
	GeneralizedRfAlgo::SplitScores calc_pairwise_split_scores(const PllSplitList &S1,
	                                                          const PllSplitList &S2);
	void
	compute_split_comparison(const PllSplit &S1, const PllSplit &S2, size_t split_len) override;
};

#endif // CONCURRENT_GROWTABLE_MCIALGO_H
