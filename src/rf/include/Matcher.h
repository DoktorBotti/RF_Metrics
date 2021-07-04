#ifndef MATCHER_H
#define MATCHER_H
#include "RectMatrix.hpp"
#include "RfAlgorithmInterface.h"
#include <future>
class Matcher {
  public:
	typedef double Scalar;
	// double solve(const RectMatrix<Scalar> &scores /*,std::vector<size_t> *best_matching_out*/);
	virtual std::future<Scalar>
	solve(RfAlgorithmInterface::SplitScores &scores) = 0;
	~Matcher();
};
#endif // MATCHER_H
