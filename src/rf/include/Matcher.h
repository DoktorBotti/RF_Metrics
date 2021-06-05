#ifndef INFORF_MATCHER_H
#define INFORF_MATCHER_H
#include "RectMatrix.hpp"
#include <vector>

class Matcher {
  public:
    virtual double solve(const RectMatrix<double> &scores, std::vector<size_t> *best_matching_out) = 0;
};
#endif // INFORF_MATCHER_H
