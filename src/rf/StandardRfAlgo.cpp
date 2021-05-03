//
// Created by Tobias Bodmer on 02.05.2021.
//

#include "include/StandardRfAlgo.h"
#include <allocator/alignedallocator.hpp>
#include <data-structures/hash_table_mods.hpp>
RfMetricInterface::Results StandardRfAlgo::calculate(std::vector<PllTree> & trees) {
	std::vector<PllSplitList> splits_list;

	for (auto & t : trees) {
		t.alignNodeIndices(*trees.begin());
		splits_list.emplace_back(t);
	}
	return RfMetricInterface::Results(trees.size());
}
