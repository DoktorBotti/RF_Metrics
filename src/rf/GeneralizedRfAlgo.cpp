//
// Created by Tobia on 16.05.2021.
//

#include "include/GeneralizedRfAlgo.h"
#include <boost/math/special_functions/factorials.hpp>
// boost logging
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>

// google linear eq solver
#include <ortools/linear_solver/linear_solver.h>

GeneralizedRfAlgo::GeneralizedRfAlgo() {
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("generalized_RF"));
}
RfMetricInterface::Results GeneralizedRfAlgo::calculate(std::vector<PllTree> &trees) {
	assert(trees.size() >= 2);
	// extract splits. Each tree now identifies by its index in all_splits
	std::vector<PllSplitList> all_splits;
	all_splits.reserve(trees.size());
	for (auto &t : trees) {
		t.alignNodeIndices(*trees.begin());
		all_splits.emplace_back(t);
	}
	RfMetricInterface::Results res(trees.size());
	double total_dst = 0;

	// iterate through all tree combinations
	for (auto a = std::as_const(all_splits).begin(); a != all_splits.end(); ++a) {
		size_t idx_a = a - all_splits.begin();
		for (auto b = a; b != all_splits.end(); ++b) {
			size_t idx_b = b - a;
			double dst = calc_tree_score(*a, *b);
			res.pairwise_distances_relative.set_at(idx_b, idx_a, dst);
			total_dst += dst;
		}
	}
	// calc mean distance between trees
	res.mean_distance = total_dst / static_cast<double>(trees.size());
	return res;
}
double GeneralizedRfAlgo::phylogenetic_prob(const PllSplit &split_a, const PllSplit &split_b) {
	using boost::math::double_factorial;
	const size_t split_len = PllSplit::split_len;
	const size_t pop_1 = split_a.popcount(split_len);
	const size_t pop_2 = split_b.popcount(split_len);
	// no trivial splits here
	assert(pop_1 > 2);
	assert(pop_1 < split_len - 1);
	assert(pop_2 > 2);
	assert(pop_2 < split_len - 1);
	// calc count
	const auto b1_fac = double_factorial<double>(2 * (split_len - pop_1) - 3);
	const auto a1_fac = double_factorial<double>(2 * pop_1 - 3);
	const auto comb_fac = double_factorial<double>(2 * pop_1 - 2 * pop_2 - 1);
	const auto dividend = double_factorial<double>(2 * split_len - 5);
	// TODO: watch out for numerical issues
	return b1_fac * a1_fac * comb_fac / dividend;
}
double GeneralizedRfAlgo::phylogenetic_prob(const PllSplit &split) {
	using boost::math::double_factorial;
	const size_t split_len = PllSplit::split_len;
	const size_t pop_1 = split.popcount(split_len);
	// no trivial splits here
	assert(pop_1 > 2);
	assert(pop_1 < split_len - 1);

	const auto b1_fac = double_factorial<double>(2 * (split_len - pop_1) - 3);
	const auto a1_fac = double_factorial<double>(2 * pop_1 - 3);
	const auto dividend = double_factorial<double>(2 * split_len - 5);
	return b1_fac * a1_fac / dividend;
}
double GeneralizedRfAlgo::calc_tree_score(const PllSplitList &A, const PllSplitList &B) {
	using namespace operations_research;
	auto scores = calc_pairwise_split_scores(A, B);
	std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
	if (!solver) {
		BOOST_LOG_SEV(logger, lg::critical) << "Could not create linear solver";
	}
	// TODO: extract everything to class scope except objective function. -Can easily be reused.
	const int num_vars = static_cast<int>(scores.size() * scores.size());
	std::vector<MPVariable *> variables;
	variables.reserve(num_vars);
	solver->MakeIntVarArray(num_vars, 0, 1, "split", &variables);

	for (size_t i = 0; i < scores.size(); ++i) {
		MPConstraint *const to_constraint = solver->MakeRowConstraint(1., 1.);
		MPConstraint *const from_constraint = solver->MakeRowConstraint(1., 1.);
		for (size_t j = 0; j < scores.size(); ++j) {
			// create restriction that each source is only used once
			from_constraint->SetCoefficient(variables[j * scores.size() + i], 1.);
			// create restriction that each target is only used once
			to_constraint->SetCoefficient(variables[i * scores.size() + j], 1.);
		}
	}

	BOOST_LOG_SEV(logger, lg::notification) << "Number of variables: " << solver->NumVariables();
	BOOST_LOG_SEV(logger, lg::notification)
	    << "Number of constraints: " << solver->NumConstraints();

	MPObjective *const objective = solver->MutableObjective();
	// set weights for target function
	for (size_t i = 0; i < scores.size(); ++i) {
		// set diagonal once
		objective->SetCoefficient(variables[i*scores.size()+i], scores.at(i,i));
		// set upper and lower triangle
		for (size_t j = 0; j < i; ++j) {
			const auto &weight = scores.at(i, j);
			// i->j and j->i have same score
			objective->SetCoefficient(variables[i * scores.size() + j], weight);
			objective->SetCoefficient(variables[j * scores.size() + i], weight);
		}
	}
	objective->SetMaximization();

	const MPSolver::ResultStatus result_status = solver->Solve();
	if (result_status != MPSolver::OPTIMAL) {
		BOOST_LOG_SEV(logger, lg::critical) << "Could not produce optimal matching!";
		return 0.;
	}
	std::stringstream out;
	std::vector<size_t> mapping(scores.size(), 0);
	std::set<size_t> connected;
	double total_score = 0;
	// get solution
	for (size_t i = 0; i < scores.size(); ++i) {
		for (size_t j = 0; j < scores.size(); ++j) {
			const auto &var = variables[i * scores.size() + j];
			if (var->solution_value() == 0) {
				continue;
			}
			assert(var->solution_value() == 1.);
			if (connected.find(i) == connected.end()) {
				// assert(mapping[i] == 0 || mapping[i] == j);
				mapping[i] = j;
				connected.insert(i);
				total_score += scores.checked_at(i, j);
			} else if (connected.find(j) == connected.end()) {
				mapping[j] = i;
				connected.insert(j);
				total_score += scores.checked_at(j, i);
			} else if (mapping[j] != i && mapping[i] != j) {
				BOOST_LOG_SEV(logger, lg::warning)
				    << "Variable mapping makes no sense for (" << i << "," << j << ")";
			}
		}
	}
	for (size_t i = 0; i < mapping.size(); ++i) {
		out << " " << i << "<>" << mapping[i] << " ";
	}
	BOOST_LOG_SEV(logger, lg::notification) << "Mapping solution: " << out.str();
	return total_score;
}
