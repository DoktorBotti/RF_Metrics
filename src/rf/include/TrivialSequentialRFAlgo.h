//
// Created by Robert Schmoltzi on 03.05.21.
//

#ifndef CONCURRENT_GROWTABLE_TRIVIALSEQEUENTIALRFALGO_H
#define CONCURRENT_GROWTABLE_TRIVIALSEQEUENTIALRFALGO_H

#include "RfAlgorithmInterface.h"
class TrivialSequentialRFAlgo : public RfAlgorithmInterface {
  public:
	RfMetricInterface::Results calculate(std::vector<PllTree> &trees) override;

  private:
	static size_t rf_dist(const PllTree &t0, const PllTree &t1);
	static bool splits_contain(const PllSplitList &splits, const PllSplit &split, size_t len);
};

#endif // CONCURRENT_GROWTABLE_TRIVIALSEQEUENTIALRFALGO_H
