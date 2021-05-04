
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
	for (const auto &list_el : splits_list) {
		for (size_t split_num = 0; split_num < num_inner_splits; ++split_num) {
			HashmapKey key(list_el.getPtrToNthElem(split_num));
			auto iter = map.find(key);
			if (iter != map.end()) {
				// set the bitvector value at tree_index
				iter->second.set(list_el.getTreeId());
			} else {
				// sets a singular bit to initialize the bitset
				size_t init_val = 1 << list_el.size();
				// allcoate bitvetcor and store a single value at tree_index
				boost::dynamic_bitset<> bits(trees.size() + 1, init_val);
				map.insert(std::make_pair(key, std::move(bits)));
			}
		}
	}

	BOOST_LOG_SEV(logger, lg::normal) << "PREPARE FOR TEXT-WALL!";
	for (const auto &el : map) {
		BOOST_LOG_SEV(logger, lg::normal) << "num ones in bitvect: " << el.second.count();
	}
	BOOST_LOG_SEV(logger, lg::normal) << "Counting pairwise tree matches";
	SymmetricMatrix<size_t> num_matches(trees.size());
	for (const auto &el : map) {
		// TODO
	}

	return RfMetricInterface::Results(trees.size());
}
StandardRfAlgo::StandardRfAlgo() {
	// optionally provide a tag
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("RF_ALG"));
}
