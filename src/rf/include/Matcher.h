//
// Created by Tobia on 19.05.2021.
//

#ifndef INFORF_MATCHER_H
#define INFORF_MATCHER_H
#include "SymmetricMatrix.hpp"
#include "LoggingBackend.h"
#include <cstdio>
#include <memory>
#include <vector>
#include <ortools/linear_solver/linear_solver.h>
#include <boost/log/sources/severity_logger.hpp>


class Matcher {
  public:
	Matcher();
    // Class not easily copyable because of the unique_ptr to solver
    Matcher(Matcher&&) = default;
	// solves the matching given pairwise scores between sets
    double solve(const SymmetricMatrix<double>& scores, std::vector<size_t>* best_matching_out);


  private:
	void init_constraints(size_t elems);

	bool is_ready = false;
	std::unique_ptr<operations_research::MPSolver> solver;
    std::vector<operations_research::MPVariable *> variables;
    operations_research::MPObjective * objective;
    boost::log::sources::severity_logger<lg::SeverityLevel> logger;


};

#endif // INFORF_MATCHER_H
