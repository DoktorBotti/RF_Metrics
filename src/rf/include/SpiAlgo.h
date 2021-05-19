//
// Created by Robert Schmoltzi on 19.05.21.
//

#ifndef CONCURRENT_GROWTABLE_SPIALGO_H
#define CONCURRENT_GROWTABLE_SPIALGO_H

#include "GeneralizedRfAlgo.h"
class SpiAlgo : public GeneralizedRfAlgo {
  protected:
    double calc_tree_score(const PllSplitList &S1, const PllSplitList &S2) override;
    SymmetricMatrix<double> calc_pairwise_split_scores(const PllSplitList &S1,
                                                       const PllSplitList &S2) override;
	static double p_phy(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len);
	static double p_phy(const PllSplit &S, size_t taxa, size_t split_len);
	static double h_info_content(const PllSplit &S, size_t taxa, size_t split_len);
	static double h_info_content(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len);
	/* Calculates whether the two splits are compatible. */
	static bool compatible(const PllSplit &S1, const PllSplit &S2, size_t split_len);
	static double calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len);
};

#endif // CONCURRENT_GROWTABLE_SPIALGO_H
