#ifndef INFORF_MATCHER_H
#define INFORF_MATCHER_H
#include "SymmetricMatrix.hpp"
#include <vector>

class Matcher {
  public:
    virtual double solve(const SymmetricMatrix<double> &scores, std::vector<size_t> *best_matching_out) = 0;
};
#endif // INFORF_MATCHER_H
