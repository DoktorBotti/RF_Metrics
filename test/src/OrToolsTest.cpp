#include "../../src/rf/include/RfMetricInterface.h"
#include "MatcherOrTools.h"
#include <RfAlgorithmInterface.h>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/math/special_functions/factorials.hpp>
#include <catch2/catch.hpp>
#include <filesystem>
#include <iostream>
#include <ortools/linear_solver/linear_solver.h>
#include <random>
#include <rf/helpers/Util.h>
static void BasicExample();
TEST_CASE("execute or-tools example", "[OR_TOOLS]") {
	// forward declare to put function at end of file
	BasicExample();
}
TEST_CASE("perform matching of specific dst matrix", "[OR_TOOLS]") {
	RectMatrix<double> dst_mtx = Util::parse_mtx_from_r("/rf_metrics/smallmtx", '\n', ' ');
	// maybe only correct results when elements are applied with log2(el)? -> No
	//	for(size_t row = 0; row < dst_mtx.size(); ++row){
	//		for(size_t col = 0; col <= row; ++col){
	//			dst_mtx.set_at(row, col, std::log2(dst_mtx.at(row,col)));
	//		}
	//	}
	MatcherOrTools matcher;
	RfAlgorithmInterface::SplitScores scores(std::move(dst_mtx));
	double res = matcher.solve(scores);
	REQUIRE(res > 0);
	// std::stringstream out;
	//	for (size_t i = 0; i < res_matching.size(); ++i) {
	//		out << res_matching[i] << ", ";
	//	}
	//	std::cout << out.str() << std::endl << "summed score: " << res;
}
TEST_CASE("matching between sample", "[OR_TOOLS]") {
	const size_t dim_size = 9;
	std::cout << "number of possible matchings: " << dim_size
	          << "! = " << boost::math::double_factorial<double>(dim_size) << "\n";
	RectMatrix<double> dst_mtx = Util::create_random_mtx(dim_size);
	MatcherOrTools matcher;
	RfAlgorithmInterface::SplitScores scores(std::move(dst_mtx));
	double res = matcher.solve(scores);
	INFO(res)
	REQUIRE(std::abs(res - 1.) >= 1e-5); // TODO: whaaa?
	// create multiple random mappings
	double closest_guess = 0;
	for (size_t rnd_it = 0; rnd_it < 50000; ++rnd_it) {
		std::vector<size_t> bad_mapping;
		std::random_device rnd;
		std::mt19937 mt(rnd());
		for (size_t i = 0; i < dim_size; ++i) {
			bad_mapping.push_back(i);
		}
		std::shuffle(bad_mapping.begin(), bad_mapping.end(), mt);
		// verify that res is (probably) best mapping
		double oth_score = 0;
		for (size_t i = 0; i < dim_size; ++i) {
			oth_score += dst_mtx.at(i, bad_mapping[i]);
		}
		if (oth_score > closest_guess) {
			closest_guess = oth_score;
		}
		REQUIRE(oth_score <= res);
	}
	std::cout << "best random guess: " << closest_guess;
}
std::size_t number_of_files_in_directory(std::filesystem::path path);
double get_pairwise_tree_score(std::filesystem::path path);
static std::string getRefPath(const RfMetricInterface::Metric &metric);
TEST_CASE("validate matching on reference pairwise scores", "[OR_TOOLS][REF]")
{
	boost::log::sources::severity_logger<lg::SeverityLevel> logger;

	std::string base_path = "../test/samples/";
	RfMetricInterface::Metric metric =
	    GENERATE(RfMetricInterface::SPI, RfMetricInterface::MSI, RfMetricInterface::MCI);
	std::string reference_path = getRefPath(metric);
	BOOST_LOG_SEV(logger, lg::normal) << "Checking inside metric path " << reference_path;
	// init randomness for later
	std::random_device rnd;
	std::mt19937 mt(rnd());
	// now iterate over all inner directories, and pick ONE random tree score to validate against
	for (const auto &sample_folder : std::filesystem::directory_iterator(reference_path)) {
		// skip irrelevant files
		if (!sample_folder.is_directory()) {
			continue;
		}
		int num_pairwise_scores =
		    static_cast<int>(number_of_files_in_directory(sample_folder.path())) - 1;
		std::uniform_int_distribution<int> distr(0, num_pairwise_scores);
		// pick random pairwise score file
		int file_idx = distr(mt);
		auto score_file_iter = std::filesystem::directory_iterator(sample_folder.path());
		std::advance(score_file_iter, file_idx);
		BOOST_LOG_SEV(logger, lg::normal)
		    << "Processing split file " << score_file_iter->path().filename() << " in folder "
		    << sample_folder.path().filename();
		double solution = get_pairwise_tree_score(score_file_iter->path());

		// calculate own solution by matching from pairwise split score matrix
		auto split_scores = Util::parse_mtx_from_r(score_file_iter->path().string(), '\n', ' ');
		MatcherOrTools matcher;
		RfAlgorithmInterface::SplitScores scores(std::move(split_scores));
		auto our_solution = matcher.solve(scores);
		double difference = std::abs(our_solution - solution);
		bool correct = difference <= 1e-3;
		CHECK(correct);
		BOOST_LOG_SEV(logger, lg::normal) << (correct ? "correct." : "FAIL");
	}
}
static std::string getRefPath(const RfMetricInterface::Metric &metric) {
	std::string reference_path = "/rf_metrics/";
	switch (metric) {
		case RfMetricInterface::SPI:
			reference_path += "SPI_10/";
			break;
		case RfMetricInterface::MSI:
			reference_path += "MSI_10/";
			break;
		case RfMetricInterface::MCI:
			reference_path += "MCI_10/";
			break;
		case RfMetricInterface::RF:
			throw std::invalid_argument("softwipe...");
	}
	return reference_path;
}

TEST_CASE("matcher arc creation", "[OR_TOOLS]") {
	MatcherOrTools matcher;
	constexpr size_t mtx_dim = 3;
	auto mtx = Util::create_random_mtx(mtx_dim);
	auto graph = matcher.getGraphCopy(mtx);

	CHECK(graph.num_nodes() == mtx_dim * 2);
	CHECK(graph.num_arcs() == mtx_dim * mtx_dim);
	size_t counter = 0;
	// Check if outdegree is always equal at left nodes
	for (const auto node : graph.AllNodes()) {
		CHECK(graph.OutDegree(node) == mtx_dim);
		++counter;
		if (counter == mtx_dim) {
			break;
		}
	}
}

std::size_t number_of_files_in_directory(std::filesystem::path path) {
	using std::filesystem::directory_iterator;
	return static_cast<size_t>(std::distance(directory_iterator(path), directory_iterator{}));
}
double get_pairwise_tree_score(std::filesystem::path path) {
	// use filename to figure out pairwise tree comparison
	std::string split_score_name = path.filename();
	auto splitted = Util::split(split_score_name, '_');
	REQUIRE(splitted.size() == 2);
	// R is a 1-starting language...
	size_t row = std::stoul(splitted[0]) - 1;
	size_t col = std::stoul(splitted[1]) - 1;

	// open pairwise score file, which is in the same directory
	path.remove_filename();
	std::string tree_score_name = path.string() + "pairwise_trees";
	auto res_mtx = Util::parse_mtx_from_r(tree_score_name, '\n', ',');
	return res_mtx.at(row, col);
}

static void BasicExample() {
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
