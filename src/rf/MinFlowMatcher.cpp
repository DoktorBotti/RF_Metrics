#include "MinFlowMatcher.h"
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>

Matcher::Scalar MinFlowMatcher::solve(const SymmetricMatrix<Matcher::Scalar> &scores,
                             std::vector<size_t> *best_matching_out) {
	// initialize on first usage
	if (!is_ready) {
		init(scores.size());
		is_ready = true;
	}
	// find maximal pairwise score, will be needed since the solver minimizes cost
	using namespace operations_research;
	// Construct the LinearSumAssignment.
	BOOST_LOG_SEV(logger, lg::normal) << "Creating assignment";
	int num_left_nodes = static_cast<int>(scores.size());
	LinearSumAssignment<Graph> a(graph, num_left_nodes);
	parameterize_assignment(scores, large_num, a);

	// Compute the optimum assignment.
	BOOST_LOG_SEV(logger, lg::normal) << "Calculating assignment";
	assert(a.FinalizeSetup());
	bool success = a.ComputeAssignment();
	assert(success);
	if (success) {
		BOOST_LOG_SEV(logger, lg::normal) << "Finished assignment sucessfully.";
	} else {
		BOOST_LOG_SEV(logger, lg::error) << "Finished assignment with errors.";
	}
	// Retrieve the cost of the optimum assignment.
	Matcher::Scalar optimum_cost = -a.GetCost() / static_cast<double>(large_num);
	Matcher::Scalar summed_cost = 0;
	// Retrieve the node-node correspondence of the optimum assignment and the
	// cost of each node pairing.
	for (int left_node = 0; left_node < num_left_nodes; ++left_node) {
		const size_t right_idx = a.GetMate(left_node) - scores.size();
		assert(right_idx >= 0 && right_idx < scores.size());
		best_matching_out->operator[](left_node) = right_idx;
		Matcher::Scalar arc_score = scores.checked_at(left_node, right_idx);
		{
			// DEBUG verify that chosen arc index is left_id * size + right_id - size
			auto arc_mate_id = a.GetAssignmentArc(left_node);
			int arc_should_id = left_node * scores.size() + right_idx;
			assert(arc_mate_id == arc_should_id);
			// DEBUG if arc indices were as they should be
			auto arc_score_diff = std::abs(arc_score + static_cast<Matcher::Scalar>( a.GetAssignmentCost(left_node)) / static_cast<Matcher::Scalar>( large_num));
			assert(arc_score_diff < 1e-6);
		}
		summed_cost += arc_score;
	}
	BOOST_LOG_SEV(logger, lg::normal) << "Total score result by summing: " << summed_cost;
	//	BOOST_LOG_SEV(logger, lg::normal) << "Total score result by OrTools directly: " <<
	// optimum_cost
	//	                                  << " difference: " <<
	// std::abs(static_cast<double>(summed_cost - optimum_cost));

	return summed_cost;
}
operations_research::LinearSumAssignment<MinFlowMatcher::Graph> &
MinFlowMatcher::parameterize_assignment(
    const SymmetricMatrix<Matcher::Scalar> &scores,
    const long large_num,
    operations_research::LinearSumAssignment<MinFlowMatcher::Graph> &a) { // tiny lambda helpers
	auto getScore = [large_num, scores](auto i, auto j) -> long {
		// multiply score with a high value to make rounding errors less troublesome
		Matcher::Scalar score = -scores.at(i, j) * large_num;
		return static_cast<long>(score);
	};
	for (int from = 0; from < scores.size() ; ++from) {
		for (int to = 0; to < from; ++to) {
			// TODO problem that casted to long?
			long arc_cost = getScore(from, to);
			// from -> to
			assign_permuted_cost(from * scores.size() + to, arc_cost, a);
			// to -> from
			assign_permuted_cost(to * scores.size() + from, arc_cost, a);
		}
		// assign diagonal cost once
		assign_permuted_cost(from * scores.size() + from, getScore(from, from), a);
	}
	return a;
}
MinFlowMatcher::MinFlowMatcher() {
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("MinFlowMatcher"));
}
void MinFlowMatcher::assign_permuted_cost(
    size_t unpermuted_index,
    long cost,
    operations_research::LinearSumAssignment<Graph> &assignment) {
	int idx = static_cast<int>(unpermuted_index);
	int perm_id = arc_permutation.empty() ? idx : arc_permutation[idx];
	assignment.SetArcCost(perm_id, cost);
}
void MinFlowMatcher::init(size_t num_matches) {
	using namespace operations_research;
	BOOST_LOG_SEV(logger, lg::normal) << "Initializing...";

	const size_t num_nodes = 2 * num_matches;
	const size_t num_arcs = num_matches * num_matches;
	graph = Graph(static_cast<int>(num_nodes), static_cast<int>(num_arcs));
	// create edges
	for (int from = 0; from < num_matches; ++from) {
		for (int to = num_matches; to < num_nodes; ++to) {
			const int arc_tail = from; // must be in [0, num_left_nodes)
			const int arc_head = to;   // must be in [num_left_nodes,
			                           // num_nodes)
			graph.AddArc(arc_tail, arc_head);
		}
	}
	// Build the StaticGraph.
	graph.Build(&arc_permutation);
	BOOST_LOG_SEV(logger, lg::normal) << "Built static graph with " << num_nodes << " nodes.";
}
void MinFlowMatcher::debugAssignment(const SymmetricMatrix<Matcher::Scalar> &scores, operations_research::LinearSumAssignment<MinFlowMatcher::Graph>* out) {
	using namespace operations_research;
    // initialize on first usage
    if (!is_ready) {
        init(scores.size());
        is_ready = true;
    }
    parameterize_assignment(scores, 2 << 30, *out);

}
MinFlowMatcher::Graph MinFlowMatcher::getGraphCopy(const SymmetricMatrix<Matcher::Scalar> &scores) {
    using namespace operations_research;
    // initialize on first usage
    if (!is_ready) {
        init(scores.size());
        is_ready = true;
    }
	return graph;
}
