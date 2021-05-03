//
// Created by Tobias Bodmer on 02.05.2021.
//

#include "include/StandardRfAlgo.h"
#include <allocator/alignedallocator.hpp>
#include <data-structures/hash_table_mods.hpp>
SymmetricMatrix<double> StandardRfAlgo::calculate(std::vector<PllTree> &trees) {
    std::vector<PllSplitList> splits_list;

    for (auto &t : trees) {
        t.alignNodeIndices(*trees.begin());
        splits_list.emplace_back(t);
    }


}
