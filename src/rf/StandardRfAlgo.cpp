
#include "include/StandardRfAlgo.h"

#include <boost/log/sources/record_ostream.hpp>
#include <boost/pending/disjoint_sets.hpp>

// needed to add a tag
#include <boost/log/attributes/constant.hpp>

RfMetricInterface::Results StandardRfAlgo::calculate(std::vector<PllTree> &trees) {
	// extract splits
	std::vector<PllSplitList> splits_list;
	size_t curr_tree = 0;
	assert(!trees.empty());
	for (auto &t : trees) {
		t.alignNodeIndices(*trees.begin());
		splits_list.emplace_back(t);
		splits_list.back().setTreeId(curr_tree++);
	}
	// define hashmap for counting similar splits
	BOOST_LOG_SEV(logger, lg::normal) << "Starting with HashMap insertion";
	// this occurs once and determines how long the splits are and consequently,
	// how much elements are part of hashing in splits
	PllSplit::split_len = splits_list[0].computeSplitLen();

	const size_t num_inner_splits = splits_list[0].size();
	const size_t num_bitvec_entries = trees.size() + 1;

	auto map = insert_all_splits(splits_list, num_inner_splits, num_bitvec_entries);

	BOOST_LOG_SEV(logger, lg::normal) << "Counting pairwise tree matches";

	size_t max_pairwise_dst = 2 * splits_list[0].size();
	// max_pairwise_dst eq to 2 * (num taxa - 3);
	size_t summed_dst = 0;
	auto pairwise_dst = pairwise_occurences(trees, map);

	auto unique_trees = calc_rf_and_unique_trees(trees.size(), pairwise_dst, max_pairwise_dst, summed_dst);

	double mean_dst =
	    static_cast<double>(summed_dst) /
	    static_cast<double>((trees.size() * (trees.size() - 1) * max_pairwise_dst) >> 1);
	BOOST_LOG_SEV(logger, lg::normal) << "Done. Mean distance: " << mean_dst;
	RfMetricInterface::Results res(trees.size());
	res.pairwise_distances_absolute = pairwise_dst;
	for (size_t i = 0; i < res.pairwise_distances_absolute.size(); ++i) {
		for (size_t j = 0; j < i; ++j) {
			res.pairwise_distances_relative.set_at(
			    i,
			    j,
			    static_cast<double>(res.pairwise_distances_absolute.at(i, j)) /
			        static_cast<double>(max_pairwise_dst));
		}
	}
	res.mean_distance = mean_dst;
	res.num_unique_trees = unique_trees;

	return res;
}

size_t StandardRfAlgo::calc_rf_and_unique_trees(const size_t tree_size,
                                                SymmetricMatrix<size_t> &pairwise_dst,
                                                const size_t max_pairwise_dst,
                                                size_t &summed_dst) {
	std::vector<size_t> rank(tree_size);
	std::vector<size_t> parent(tree_size);
	boost::disjoint_sets<size_t *, size_t *> d_set(&rank[0], &parent[0]);
	for (size_t i = 0; i < tree_size; ++i) {
		d_set.make_set(i);
	}

	// Calculates the RF metric by subtracting two times the intersection size from the sum of both
	// sizes.
	for (size_t i = 0; i < tree_size; ++i) {
		// init distance to maximum
		for (size_t j = 0; j < i; ++j) {
			size_t tmp = max_pairwise_dst - (pairwise_dst.at(i, j) << 1); // * 2
			assert(tmp < max_pairwise_dst);
			pairwise_dst.set_at(i, j, tmp); // TODO: Count 0's or 1's timing

			summed_dst += tmp;

			if (tmp == 0) {
				d_set.union_set(i, j);
			}
		}
	}

	// TODO: WTF is there no simpler way?
	std::vector<size_t> tree_reps(tree_size);
	std::iota(tree_reps.begin(), tree_reps.end(), 0);
	return d_set.count_sets(tree_reps.begin(), tree_reps.end());
}

SymmetricMatrix<size_t> StandardRfAlgo::pairwise_occurences(
    const std::vector<PllTree> &trees,
    const std::unordered_map<HashmapKey, boost::dynamic_bitset<>, HashingFunctor> &map) {
	SymmetricMatrix<size_t> pairwise_dst(trees.size());
	// Increments each matrix entry for each split which is in both trees.
	for (const auto &el : map) {
		for (size_t i = 0; i < trees.size(); ++i) {
			if (!el.second.test(i)) {
				continue;
			}
			BOOST_LOG_SEV(logger, lg::normal) << "Row " << i << " of " << trees.size();

			for (size_t j = 0; j < i; ++j) {
				if (el.second.test(j)) {
					size_t old_val = pairwise_dst.at(i, j);
					pairwise_dst.set_at(i, j, old_val + 1);
				}
			}
		}
	}
	return pairwise_dst;
}

std::unordered_map<HashmapKey, boost::dynamic_bitset<>, HashingFunctor>
StandardRfAlgo::insert_all_splits(const std::vector<PllSplitList> &split_lists,
                                  const size_t num_inner_splits,
                                  const size_t num_bitvec_entries) {
	// Adds all splits into a hashmap. The value is a bitvector whose 1's represent all trees which
	// contain this split.
	std::unordered_map<HashmapKey, boost::dynamic_bitset<>, HashingFunctor> map;
	for (const auto &list_el : split_lists) {
		for (size_t split_num = 0; split_num < num_inner_splits; ++split_num) {
			HashmapKey key(list_el.getPtrToNthElem(split_num)); // TODO: Should be PllSplit (and not
			// const *)
			auto iter = map.find(key);
			if (iter != map.end()) {
				// set the bitvector value at tree_index
				iter->second.set(list_el.getTreeId()); // TODO: Tree id not needed
			} else {
				// allcoate bitvetcor and store a single value at tree_index
				boost::dynamic_bitset<> bits(num_bitvec_entries, 0);
				bits.set(list_el.getTreeId()); // TODO: push_back not needed -> faster type
				// anywhere?
				map.insert(std::make_pair(key, std::move(bits)));
			}
		}
	}
	return map;
}

StandardRfAlgo::StandardRfAlgo() {
	// optionally provide a tag
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("RF_ALG"));
}
