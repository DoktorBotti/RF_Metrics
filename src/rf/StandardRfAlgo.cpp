
#include "include/StandardRfAlgo.h"
#include <boost/dynamic_bitset.hpp>

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

	std::unordered_map<HashmapKey, boost::dynamic_bitset<>, HashingFunctor> map;
	const size_t num_inner_splits = splits_list[0].size();
	const size_t num_bitvec_entries = trees.size() + 1;

	// Adds all splits into a hashmap. The value is a bitvector whose 1's represent all trees which
	// contain this split.
	for (const auto &list_el : splits_list) {
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

	BOOST_LOG_SEV(logger, lg::normal) << "PREPARE FOR TEXT-WALL!";

	BOOST_LOG_SEV(logger, lg::normal) << "Counting pairwise tree matches";
	SymmetricMatrix<size_t> pairwise_dst(trees.size());
	// max_pairwise_dst eq to 2 * (num taxa -3)
	size_t max_pairwise_dst = 2 * splits_list[0].size();
	size_t summed_dst = 0;

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

	std::vector<size_t> rank(trees.size());
	std::vector<size_t> parent(trees.size());
	boost::disjoint_sets<size_t *, size_t *> d_set(&rank[0], &parent[0]);
	for (size_t i = 0; i < trees.size(); ++i) {
		d_set.make_set(i);
	}

	// Calculates the RF metric by subtracting two times the intersection size from the sum of both
	// sizes.
	for (size_t i = 0; i < trees.size(); ++i) {
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
	std::vector<size_t> tree_reps(trees.size());
	std::iota(tree_reps.begin(), tree_reps.end(), 0);
	size_t unique_trees = d_set.count_sets(tree_reps.begin(), tree_reps.end());

	double mean_dst =
	    static_cast<double>(summed_dst) /
	    static_cast<double>((trees.size() * (trees.size() - 1) * max_pairwise_dst) >> 1);
	BOOST_LOG_SEV(logger, lg::normal) << "Done. Mean distance: " << mean_dst;
	RfMetricInterface::Results res(trees.size());
	res.pairwise_distances = pairwise_dst;
	res.mean_distance = mean_dst;
	res.num_unique_trees = unique_trees;

	return res;
}
StandardRfAlgo::StandardRfAlgo() {
	// optionally provide a tag
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("RF_ALG"));
}
