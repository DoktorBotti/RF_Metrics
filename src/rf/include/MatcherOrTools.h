#ifndef ORTOOLS_MATCHER_H
#define ORTOOLS_MATCHER_H
#include "LoggingBackend.h"
#include "Matcher.h"
#include "RectMatrix.hpp"
#include "ortools/graph/linear_assignment.h"
#include <boost/log/sources/severity_logger.hpp>
#include <ortools/graph/graph.h>
#include <vector>

class MatcherOrTools : public Matcher {
  public:
	typedef util::StaticGraph<> Graph;

	explicit MatcherOrTools();
	~MatcherOrTools() override = default;
	// double solve(const RectMatrix<Scalar> &scores /*,std::vector<size_t> *best_matching_out*/);
	std::future<MatcherOrTools::Scalar> solve(RfAlgorithmInterface::SplitScores &scores) override;

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
	static void assign_permuted_cost(size_t unpermuted_index,
	                                 long cost,
	                                 operations_research::LinearSumAssignment<Graph> &assignment,
	                                 const std::vector<int> &arc_permutations);

	//	operations_research::LinearSumAssignment<Matcher::Graph> &
	//	parameterize_assignment(const RectMatrix<Scalar> &scores,
	//	                        operations_research::LinearSumAssignment<Matcher::Graph> &a);
	static operations_research::LinearSumAssignment<MatcherOrTools::Graph> &
	parameterize_assignment(const RfAlgorithmInterface::SplitScores &scores,
	                        operations_research::LinearSumAssignment<MatcherOrTools::Graph> &a,
	                        double lap_factor);
	static double parallel_calc(RfAlgorithmInterface::SplitScores &scores,
	                            const MatcherOrTools::Graph &graph);
};

#endif // ORTOOLS_MATCHER_H
