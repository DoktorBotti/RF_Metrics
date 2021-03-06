//
// Created by Robert Schmoltzi on 19.05.21.
//

#ifndef CONCURRENT_GROWTABLE_SPIALGO_H
#define CONCURRENT_GROWTABLE_SPIALGO_H

#include "GeneralizedRfAlgo.h"
class SpiAlgo : public GeneralizedRfAlgo {
  public:
	double calc_split_score(const PllSplit &S1,
	                        const PllSplit &S2) override;
	SpiAlgo() = default;
	SpiAlgo(SpiAlgo &&) = default;
	~SpiAlgo() override = default;

	explicit SpiAlgo(size_t split_len);
    RfAlgorithmInterface::Scalar calc_split_score(const PllSplit &S1) override;

  private:
	static Scalar one_overlap(size_t a, size_t b, size_t n);
	static Scalar one_overlap_notb(size_t a, size_t n_minus_b, size_t n);
};

#endif // CONCURRENT_GROWTABLE_SPIALGO_H
