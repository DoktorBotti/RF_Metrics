//
// Created by Tobia on 19.05.2021.
//

#include "NaiveMatcher.h"
// boost logging
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>
NaiveMatcher::NaiveMatcher() {
    using namespace operations_research;
    logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("NaiveMatcher"));
    solver.reset(MPSolver::CreateSolver("SCIP"));
    if (!solver) {
        BOOST_LOG_SEV(logger, lg::critical) << "Could not create linear solver";
    }
    // initialize objective
    objective = solver->MutableObjective();
    objective->SetMaximization();
}
Matcher::Scalar NaiveMatcher::solve(const RectMatrix<Matcher::Scalar> &scores,
                      std::vector<size_t> *best_matching_out) {
	using namespace operations_research;
	// initialize solver only once
	if (!is_ready) {
		init_constraints(scores.size());
	} else {
		// if initialized, the variable sizes must not change
		assert(variables.size() == scores.size() * scores.size());
	}
	// set weights for target function
	for (size_t i = 0; i < scores.size(); ++i) {
		// set diagonal once
		objective->SetCoefficient(variables[i * scores.size() + i], scores.at(i, i));
		// set upper and lower triangle
		for (size_t j = 0; j < i; ++j) {
			const auto &weight = scores.at(i, j);
			// i->j and j->i have same score
			objective->SetCoefficient(variables[i * scores.size() + j], weight);
			objective->SetCoefficient(variables[j * scores.size() + i], weight);
		}
	}

	const MPSolver::ResultStatus result_status = solver->Solve();
	if (result_status != MPSolver::OPTIMAL) {
		BOOST_LOG_SEV(logger, lg::critical) << "Could not produce optimal matching!";
		return -1.;
	}
	// get solution
	std::set<size_t> connected;
	Matcher::Scalar total_score = 0;
		for (size_t i = 0; i < scores.size(); ++i) {
			for (size_t j = 0; j < scores.size(); ++j) {
				const auto &var = variables[i * scores.size() + j];
				if (var->solution_value() == 0) {
					continue;
				}
				assert(var->solution_value() == 1.);
				if (connected.find(i) == connected.end()) {
					best_matching_out->operator[](i) = j;
					connected.insert(i);
					total_score += scores.at(i, j);
				} else if (connected.find(j) == connected.end()) {
					best_matching_out->operator[](j) = i;
					connected.insert(j);
					total_score += scores.at(j, i);
				} else if (best_matching_out->operator[](j) != i &&
				           best_matching_out->operator[](i) != j) {
					BOOST_LOG_SEV(logger, lg::warning)
					    << "Variable mapping makes no sense for (" << i << "," << j << ")";
					assert(false);
				}
			}
		}

	return total_score;
}
void NaiveMatcher::init_constraints(size_t elems) {
	using namespace operations_research;
	const int num_vars = static_cast<int>(elems * elems);
	variables.reserve(num_vars);
	solver->MakeIntVarArray(num_vars, 0, 1, "split", &variables);

	for (size_t i = 0; i < elems; ++i) {
		MPConstraint *const to_constraint = solver->MakeRowConstraint(1., 1.);
		MPConstraint *const from_constraint = solver->MakeRowConstraint(1., 1.);
		for (size_t j = 0; j < elems; ++j) {
			// create restriction that each source is only used once
			from_constraint->SetCoefficient(variables[j * elems + i], 1.);
			// create restriction that each target is only used once
			to_constraint->SetCoefficient(variables[i * elems + j], 1.);
		}
	}
	BOOST_LOG_SEV(logger, lg::notification) << "Number of variables: " << solver->NumVariables();
	BOOST_LOG_SEV(logger, lg::notification)
	    << "Number of constraints: " << solver->NumConstraints();
}
