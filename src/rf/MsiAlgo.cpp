//
// Created by Tobia on 16.05.2021.
//

#include "MsiAlgo.h"
#include <random>
SymmetricMatrix<double> MsiAlgo::calc_pairwise_split_scores(const PllSplitList &S1,
                                                            const PllSplitList &S2) {
	// TODO: make working sample. filling with random inputs for now.
	SymmetricMatrix<double> res(S1.size());
	std::mt19937 rand_engine(3);
	std::uniform_real_distribution<> distr(0., 500.);

	for (size_t i = 0; i < S1.size(); ++i) {
		for (size_t j = 0; j <= i; ++j) {
			res.set_at(i, j, distr(rand_engine));
		}
	}
	return res;
}
