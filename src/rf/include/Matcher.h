#ifndef INFORF_MATCHER_H
#define INFORF_MATCHER_H
#include "LoggingBackend.h"
#include "RectMatrix.hpp"
#include "RfAlgorithmInterface.h"
#include "ortools/graph/linear_assignment.h"
#include <boost/log/sources/severity_logger.hpp>
#include <ortools/graph/graph.h>

class Matcher {
  public:
	typedef double Scalar;
	typedef util::StaticGraph<> Graph;

	explicit Matcher();

	// double solve(const RectMatrix<Scalar> &scores /*,std::vector<size_t> *best_matching_out*/);
    Scalar solve(const RfAlgorithmInterface::SplitScores &scores);

	Graph getGraphCopy(const RectMatrix<Scalar> &scores);

  private:
	// important private members
	Graph graph;
	std::vector<Graph::ArcIndex> arc_permutation;
	boost::log::sources::severity_logger<lg::SeverityLevel> logger;

	// used for init on first usage
	bool is_ready = false;
	void init(size_t num_splits);
	// helper functions
	void assign_permuted_cost(size_t unpermuted_index,
	                          long cost,
	                          operations_research::LinearSumAssignment<Graph> &graph);

//	operations_research::LinearSumAssignment<Matcher::Graph> &
//	parameterize_assignment(const RectMatrix<Scalar> &scores,
//	                        operations_research::LinearSumAssignment<Matcher::Graph> &a);
	operations_research::LinearSumAssignment<Matcher::Graph> &
	parameterize_assignment(const RfAlgorithmInterface::SplitScores &scores,
	                        operations_research::LinearSumAssignment<Matcher::Graph> &a,
	                        double lap_factor);
};

#endif // INFORF_MATCHER_H
