//
// Created by Tobias Bodmer on 20.05.2021.
//

#ifndef INFORF_MINFLOWMATCHER_H
#define INFORF_MINFLOWMATCHER_H
#include "Matcher.h"
#include <boost/log/sources/severity_logger.hpp>
#include "LoggingBackend.h"
#include <ortools/graph/graph.h>
#include "ortools/graph/linear_assignment.h"


class MinFlowMatcher : public Matcher {
  public:
	explicit MinFlowMatcher();
	double solve(const SymmetricMatrix<double> &scores,
	             std::vector<size_t> *best_matching_out) override;

    typedef util::StaticGraph<> Graph;
  private:
	// important private members
	Graph graph;
    std::vector<Graph::ArcIndex> arc_permutation;
    boost::log::sources::severity_logger<lg::SeverityLevel> logger;

    // used for init on first usage
    bool is_ready = false;
	void init(size_t num_splits);
	// helper functions
	void assign_permuted_cost(size_t unpermuted_index, long cost, operations_research::LinearSumAssignment<Graph>& graph);

};

#endif // INFORF_MINFLOWMATCHER_H
