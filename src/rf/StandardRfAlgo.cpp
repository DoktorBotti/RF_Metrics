
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
			HashmapKey key(list_el.getPtrToNthElem(split_num));
			auto iter = map.find(key);
			if (iter != map.end()) {
				// set the bitvector value at tree_index
				iter->second.set(list_el.getTreeId());
			} else {
				// sets a singular bit to initialize the bitset
				size_t init_val = 1 << list_el.size();
				// allcoate bitvetcor and store a single value at tree_index
				boost::dynamic_bitset<> bits(num_bitvec_entries, init_val);
				map.insert(std::make_pair(key, std::move(bits)));
			}
		}
	}

	BOOST_LOG_SEV(logger, lg::normal) << "PREPARE FOR TEXT-WALL!";
	for (const auto &el : map) {
		//BOOST_LOG_SEV(logger, lg::normal) << "num ones in bitvect: " << el.second.count();
	}
	BOOST_LOG_SEV(logger, lg::normal) << "Counting pairwise tree matches";
	std::vector<std::unordered_map<size_t, size_t>> reduced_map(num_bitvec_entries);
	// reduce results of hashmap -- does this make sense?
	size_t count = 0;
	for (const auto &el : map) {
        BOOST_LOG_SEV(logger, lg::normal) << "Working on " << ++count << "th element of " << map.size();
        if (!el.second.any()) {
			// counter should never be empty
			assert(false);
			continue;
		}
		for (size_t i = 0; i < num_bitvec_entries; ++i) {
			if (el.second.test(i)) {
				continue;
			}
            BOOST_LOG_SEV(logger, lg::normal) << "Inner element "<< i << " of " << num_bitvec_entries;

            auto &inner_map = reduced_map[i];
			for (size_t j = i + 1; j < num_bitvec_entries - 1; ++j) {
				if (el.second.test(j)) {
					continue;
				}
				auto iter = inner_map.find(j);
				if (iter != inner_map.end()) {
					iter->second += 1;
				} else {
					inner_map.insert(std::make_pair(j, 1));
				}
			}
		}
	}
    BOOST_LOG_SEV(logger, lg::normal) << "Creating pairwise matrix";
    SymmetricMatrix<size_t> num_matches(trees.size());

    // match every tree i to j -- did i loose information by reducing before?
	for (size_t i = 0; i < num_bitvec_entries - 1; ++i) {
		const auto& map = reduced_map[i];
		for(const auto& el : map){
			auto minEl = (i < el.first)? i : el.first;
			auto maxEl = (i < el.first)? el.first : i;
			num_matches.checked_set_at(maxEl, minEl, el.second);
		}
	}

	return RfMetricInterface::Results(trees.size());
}
StandardRfAlgo::StandardRfAlgo() {
	// optionally provide a tag
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("RF_ALG"));
}
