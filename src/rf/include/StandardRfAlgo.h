//
// Created by Tobias Bodmer on 02.05.2021.
//

#ifndef INFORF_STANDARDRFALGO_H
#define INFORF_STANDARDRFALGO_H
#include "HashingFunctor.hpp"
#include "PllTree.hpp"
#include "RfAlgorithmInterface.h"
#include <boost/log/sources/severity_logger.hpp>

#include <boost/dynamic_bitset.hpp>
#include <vector>

class StandardRfAlgo : public RfAlgorithmInterface {
  public:
	StandardRfAlgo();
	StandardRfAlgo(const StandardRfAlgo &) = default;
	StandardRfAlgo(StandardRfAlgo &&) = default;
	~StandardRfAlgo() override = default;
	RfMetricInterface::Results calculate(std::vector<PllTree> &trees) override;

  private:
	boost::log::sources::severity_logger<lg::SeverityLevel> logger;
	static std::unordered_map<HashmapKey, boost::dynamic_bitset<>, HashingFunctor>
	insert_all_splits(const std::vector<PllSplitList> &split_lists);
	SymmetricMatrix<size_t> pairwise_occurences(
	    const std::vector<PllTree> &trees,
	    const std::unordered_map<HashmapKey, boost::dynamic_bitset<>, HashingFunctor> &map);
	static size_t calc_rf_and_unique_trees(size_t tree_size,
	                                       SymmetricMatrix<size_t> &pairwise_dst,
	                                       size_t max_pairwise_dst,
	                                       size_t &summed_dst);
};

#endif // INFORF_STANDARDRFALGO_H
