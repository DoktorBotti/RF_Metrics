//
// Created by Tobia on 16.05.2021.
//

#include "MsiAlgo.h"
double MsiAlgo::calcScore(const PllSplitList &A, const PllSplitList &B) {
	assert(A.size() == B.size());
	// store best paring
	std::vector<size_t> best_pairing;
	double best_score = 0;
	// iterate through all possible split combinations
	// TODO: that is not happening yet. This only calculates pairwise scores.
	for (size_t i = 0;i < A.size(); ++i){
		for (size_t j = i; j < A.size(); ++j){
			const auto & a = A[i];
			const auto & b = B[j];
			// calculate score if compatible
			if(pllmod_utree_compatible_splits(a(),b(), PllSplit::split_len, A.size()- 1)==1){
				double prob = std::max(phylogeneticProbability(a,b), phylogeneticProbability(b,a));
				if(prob > best_score)
                {
					best_score = prob;

				}
			}
		}
	}
	// check compatibility
	// then add up a1 and a2 bzw. a1 and b2 & return maximum of that
}
