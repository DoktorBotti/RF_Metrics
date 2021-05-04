
#include "include/StandardRfAlgo.h"
#include <boost/dynamic_bitset.hpp>
#include <unordered_map>

#include <boost/log/sources/record_ostream.hpp>

// needed to add a tag
#include <boost/log/attributes/constant.hpp>
#include <iostream>
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
	for (const auto &list_el : splits_list) {
		for (size_t split_num = 0; split_num < num_inner_splits; ++split_num) {
			HashmapKey key(list_el.getPtrToNthElem(split_num)); // TODO: Should be PllSplit (and not const *)
			auto iter = map.find(key);
			if (iter != map.end()) {
				// set the bitvector value at tree_index
				iter->second.set(list_el.getTreeId()); // TODO: Tree id not needed
			} else {
				// allcoate bitvetcor and store a single value at tree_index
				boost::dynamic_bitset<> bits(num_bitvec_entries, 0);
				bits.set(list_el.getTreeId()); // TODO: push_back not needed -> faster type anywhere?
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
	for (size_t i = 0; i < trees.size(); ++i) {
		BOOST_LOG_SEV(logger, lg::normal) << "Row " << i << " of " << trees.size();
		// init distance to maximum
		for (size_t j = 0; j < i; ++j) {
			pairwise_dst.set_at(i, j, max_pairwise_dst);
		}
		for (size_t j = 0; j < i; ++j) {
			for (const auto &el : map) {
				if (el.second.test(j) && el.second.test(i)) {
                    size_t old_val = pairwise_dst.at(i, j);
                    pairwise_dst.set_at(i, j, old_val - 2);
				}
			}
			summed_dst += pairwise_dst.at(i, j);
		}
	}
	double mean_dst = static_cast<double>(summed_dst) / static_cast<double>(trees.size() * (trees.size()-1));
    BOOST_LOG_SEV(logger, lg::normal) << "Done. Mean distance: " << mean_dst;
    RfMetricInterface::Results res(trees.size());
	res.pairwise_distances = pairwise_dst;
	res.mean_distance = mean_dst;

    return res;
}
StandardRfAlgo::StandardRfAlgo() {
	// optionally provide a tag
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("RF_ALG"));
}
