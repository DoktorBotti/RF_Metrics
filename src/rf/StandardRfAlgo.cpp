
#include "include/StandardRfAlgo.h"
#include <unordered_map>

#include <boost/log/sources/record_ostream.hpp>

// needed to add a tag
#include <boost/log/attributes/constant.hpp>
#include <iostream>
RfMetricInterface::Results StandardRfAlgo::calculate(std::vector<PllTree> &trees) {
	// extract splits
	std::vector<PllSplitList> splits_list;

	for (auto &t : trees) {
		t.alignNodeIndices(*trees.begin());
		splits_list.emplace_back(t);
	}
	// define hashmap for counting similar splits
	BOOST_LOG_SEV(logger, lg::normal) << "Starting with HashMap insertion";

	PllSplit::split_len = splits_list[0].computeSplitLen();
	std::unordered_map<HashmapKey, int, HashingFunctor> map;
	const size_t num_inner_splits = splits_list.size();
	for (const auto &list_el : splits_list) {
		for (size_t split_num = 0; split_num < num_inner_splits; ++split_num) {
			HashmapKey testA(list_el[split_num]);
			HashmapKey testB(list_el[split_num]);
			HashingFunctor hasher;
			assert(hasher.operator()(testA) == hasher.operator()(HashmapKey(list_el[split_num])));

			HashmapKey key((list_el[split_num]));
			auto iter = map.find(key);
			if (iter != map.end()) {
				iter->second += 1;
			} else {
				map.insert(std::make_pair<HashmapKey, int>(HashmapKey(list_el[split_num]), 1));
				map.insert(std::make_pair<HashmapKey, int>(HashmapKey(list_el[split_num]), 1));
			}
		}
	}

	BOOST_LOG_SEV(logger, lg::normal) << "PREPARE FOR TEXT-WALL!";
	for (const auto &el : map) {
		pllmod_utree_split_show(el.first.split(), splits_list.size() + 3);
		std::cout << el.second << std::endl;
	}

	return RfMetricInterface::Results(trees.size());
}
StandardRfAlgo::StandardRfAlgo() {
	// optionally provide a tag
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("RF_ALG"));
}
