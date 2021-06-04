//
// Created by Tobias Bodmer on 20.05.2021.
//

#ifndef INFORF_MINFLOWMATCHER_H
#define INFORF_MINFLOWMATCHER_H
#include "LoggingBackend.h"
#include "Matcher.h"
#include "ortools/graph/linear_assignment.h"
#include <boost/log/sources/severity_logger.hpp>
#include <ortools/graph/graph.h>

class MinFlowMatcher : public Matcher {
  public:
	typedef util::StaticGraph<> Graph;

	explicit MinFlowMatcher();
	double solve(const SymmetricMatrix<double> &scores,
	             std::vector<size_t> *best_matching_out) override;

	void debugAssignment(const SymmetricMatrix<double> &scores,
	                     operations_research::LinearSumAssignment<MinFlowMatcher::Graph> *out);
	Graph getGraphCopy(const SymmetricMatrix<double> &scores);
	// factor for more precision after rounding, normally private but testing stuff
	static const long large_num = (long) 2 << 30;

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

	operations_research::LinearSumAssignment<MinFlowMatcher::Graph> &
	parameterize_assignment(const SymmetricMatrix<double> &scores,
	                        const long large_num,
	                        operations_research::LinearSumAssignment<MinFlowMatcher::Graph> &a);
};

#endif // INFORF_MINFLOWMATCHER_H
