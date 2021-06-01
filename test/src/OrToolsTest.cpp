#include "MinFlowMatcher.h"
#include <catch2/catch.hpp>
#include <iostream>
#include <ortools/linear_solver/linear_solver.h>
#include <random>
#include <rf/helpers/Util.h>


TEST_CASE("execute or-tools example", "[OR_TOOLS]") {
	// forward declare to put function at end of file
	void BasicExample();
	BasicExample();
}

TEST_CASE("matching between sample", "[OR_TOOLS]") {
	const size_t dim_size = 10;
	SymmetricMatrix<double> dst_mtx = Util::create_random_mtx(dim_size);
	MinFlowMatcher matcher;
	std::vector<size_t> res_matching(dim_size, 0);
	double res = matcher.solve(dst_mtx, &res_matching);
	INFO(res);
	REQUIRE(res != -1.);
	// create 100 random mappings
	int dummy = GENERATE(take(100, random(-100, 100)));
	std::vector<size_t> bad_mapping;
	std::random_device rnd;
	std::mt19937 mt(rnd()*dummy);
	for (size_t i = 0; i < dim_size; ++i) {
		bad_mapping.push_back(i);
	}
	std::shuffle(bad_mapping.begin(), bad_mapping.end(), mt);
	// verify that res is (probably) best mapping
	double oth_score = 0;
	for (size_t i = 0; i < dim_size; ++i) {
		oth_score += dst_mtx.checked_at(i, bad_mapping[i]);
	}
	REQUIRE(oth_score <= res);
}

void BasicExample() {
	using namespace operations_research;
	// Create the linear solver with the GLOP backend.
	std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("GLOP"));

	// Create the variables x and y.
	MPVariable *const x = solver->MakeNumVar(0.0, 1, "x");
	MPVariable *const y = solver->MakeNumVar(0.0, 2, "y");

	std::cout << "Number of variables = " << solver->NumVariables() << std::endl;

	// Create a linear constraint, 0 <= x + y <= 2.
	MPConstraint *const ct = solver->MakeRowConstraint(0.0, 2.0, "ct");
	ct->SetCoefficient(x, 1);
	ct->SetCoefficient(y, 1);

	std::cout << "Number of constraints = " << solver->NumConstraints() << std::endl;

	// Create the objective function, 3 * x + y.
	MPObjective *const objective = solver->MutableObjective();
	objective->SetCoefficient(x, 3);
	objective->SetCoefficient(y, 1);
	objective->SetMaximization();

	solver->Solve();

	std::cout << "Solution:" << std::endl;
	std::cout << "Objective value = " << objective->Value() << std::endl;
	std::cout << "x = " << x->solution_value() << std::endl;
	std::cout << "y = " << y->solution_value() << std::endl;
}
