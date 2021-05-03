
#include "unordered_map"
#include "include/StandardRfAlgo.h"
RfMetricInterface::Results StandardRfAlgo::calculate(std::vector<PllTree> & trees) {
	// extract splits
	std::vector<PllSplitList> splits_list;

	for (auto & t : trees) {
		t.alignNodeIndices(*trees.begin());
		splits_list.emplace_back(t);
	}
    // define hashmap for counting similar splits

	struct SimpleKey
    {
        std::string the_data;
		bool operator==(const SimpleKey &rhs) const {
			return the_data == rhs.the_data;
		}
		bool operator!=(const SimpleKey &rhs) const {
			return !(rhs == *this);
		}
	};
	 struct MyHash{
		std::size_t operator()(const SimpleKey& key) const{
			return std::hash<std::string>()(key.the_data);
		}
	};
   std::unordered_map<SimpleKey, SimpleKey, MyHash> map;
   SimpleKey key;
   key.the_data = "IT WOOOKKKKKRRRSSSSS";
   SimpleKey val = {"...maybe"};
   map.insert(std::make_pair(key,val));

	return RfMetricInterface::Results(trees.size());
}
