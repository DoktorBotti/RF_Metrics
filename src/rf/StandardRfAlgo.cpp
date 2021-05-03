//
// Created by Tobias Bodmer on 02.05.2021.
//

#include "include/StandardRfAlgo.h"
RfMetricInterface::Results StandardRfAlgo::calculate(std::vector<PllTree> & trees) {
	// extract splits
	std::vector<PllSplitList> splits_list;

	for (auto & t : trees) {
		t.alignNodeIndices(*trees.begin());
		splits_list.emplace_back(t);
	}


	return RfMetricInterface::Results(trees.size());
}
