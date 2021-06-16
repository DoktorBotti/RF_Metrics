#ifndef MATCHER_H
#define MATCHER_H
#include "RectMatrix.hpp"
#include "RfAlgorithmInterface.h"
class Matcher {
  public:
	typedef double Scalar;
	// double solve(const RectMatrix<Scalar> &scores /*,std::vector<size_t> *best_matching_out*/);
	virtual Scalar solve(const RfAlgorithmInterface::SplitScores &scores) = 0;
	~Matcher();
};
#endif // MATCHER_H
