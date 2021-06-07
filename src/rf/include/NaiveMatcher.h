//
// Created by Tobia on 19.05.2021.
//

#ifndef INFORF_NAIVEMATCHER_H
#define INFORF_NAIVEMATCHER_H
#include "LoggingBackend.h"
#include "Matcher.h"
#include <boost/log/sources/severity_logger.hpp>
#include <cstdio>
#include <memory>
#include <ortools/linear_solver/linear_solver.h>

class NaiveMatcher : public Matcher {
  public:
	NaiveMatcher();
	// Class not easily copyable because of the unique_ptr to solver
	NaiveMatcher(NaiveMatcher &&) = default;
	// solves the matching given pairwise scores between sets
	Scalar solve(const SymmetricMatrix<double> &scores, std::vector<size_t> *best_matching_out) override;

  private:
	void init_constraints(size_t elems);

	bool is_ready = false;
	std::unique_ptr<operations_research::MPSolver> solver;
	std::vector<operations_research::MPVariable *> variables;
	operations_research::MPObjective *objective;
    boost::log::sources::severity_logger<lg::SeverityLevel> logger;

    // for timing purposes
	size_t num_solves = 0;
	double time_avg = 0;
	double worst_time = 0;
};

#endif // INFORF_NAIVEMATCHER_H
