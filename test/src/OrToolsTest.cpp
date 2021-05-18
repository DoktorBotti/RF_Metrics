#include <ortools/linear_solver/linear_solver.h>
#include <catch2/catch.hpp>
#include <rf/helpers/Util.h>
#include <iostream>
#include "MsiAlgo.h"

namespace operations_research {
void BasicExample() {
    // Create the linear solver with the GLOP backend.
    std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("GLOP"));

    // Create the variables x and y.
    MPVariable* const x = solver->MakeNumVar(0.0, 1, "x");
    MPVariable* const y = solver->MakeNumVar(0.0, 2, "y");

    std::cout << "Number of variables = " << solver->NumVariables()<< std::endl;

    // Create a linear constraint, 0 <= x + y <= 2.
    MPConstraint* const ct = solver->MakeRowConstraint(0.0, 2.0, "ct");
    ct->SetCoefficient(x, 1);
    ct->SetCoefficient(y, 1);

    std::cout << "Number of constraints = " << solver->NumConstraints()<< std::endl;

    // Create the objective function, 3 * x + y.
    MPObjective* const objective = solver->MutableObjective();
    objective->SetCoefficient(x, 3);
    objective->SetCoefficient(y, 1);
    objective->SetMaximization();

    solver->Solve();

    std::cout << "Solution:" << std::endl;
    std::cout << "Objective value = " << objective->Value()<< std::endl;
    std::cout << "x = " << x->solution_value()<< std::endl;
    std::cout << "y = " << y->solution_value()<< std::endl;
}
}  // namespace operations_research

TEST_CASE("execute or-tools example", "[OR_TOOLS]"){
	operations_research::BasicExample();
}
TEST_CASE("matching between sample", "[OR_TOOLS]"){
    auto split_lists = Util::create_splits(15, "/rf_data/BS/125");
	MsiAlgo algo;
	double res = algo.calc_tree_score(split_lists[0], split_lists[10]);
    INFO(res);
    REQUIRE(false);
}