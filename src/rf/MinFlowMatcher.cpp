#include "MinFlowMatcher.h"
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>

double MinFlowMatcher::solve(const SymmetricMatrix<double> &scores,
                             std::vector<size_t> *best_matching_out) {
	// initialize on first usage
	if (!is_ready) {
		init(scores.size());
		is_ready = true;
	}
	constexpr long large_num = 2 << 20;
	// find maximal pairwise score, will be needed since the solver minimizes cost
	using namespace operations_research;
	// Construct the LinearSumAssignment.
	BOOST_LOG_SEV(logger, lg::normal) << "Creating assignment";
	int num_left_nodes = static_cast<int>(scores.size());
	LinearSumAssignment<Graph> a(graph, num_left_nodes);
	// tiny lambda helpers
	auto getScore = [large_num, scores](auto i, auto j) -> long {
		// multiply score with a high value to make rounding errors less troublesome
		auto score = -scores.at(i, j)*large_num;
		return static_cast<long>(score);
	};
	for (int from = 0; from < num_left_nodes; ++from) {
		// assign diagonal cost once
		assign_permuted_cost(from * scores.size() + from, getScore(from, from), a);

		for (int to = 0; to < from; ++to) {
			// TODO problem that casted to long?
			long arc_cost = getScore(from, to);
			// from -> to
			assign_permuted_cost(from * scores.size() + to, arc_cost, a);
			// to -> from
			assign_permuted_cost(to * scores.size() + from, arc_cost, a);
		}
	}

	// Compute the optimum assignment.
	BOOST_LOG_SEV(logger, lg::normal) << "Calculating assignment";
	bool success = a.ComputeAssignment();
	assert(success);
	if (success) {
		BOOST_LOG_SEV(logger, lg::normal) << "Finished assignment sucessfully.";
	} else {
		BOOST_LOG_SEV(logger, lg::error) << "Finished assignment with errors.";
	}
	// Retrieve the cost of the optimum assignment.
	double optimum_cost = -a.GetCost() / static_cast<double>( large_num);
	double summed_cost = 0;
	// Retrieve the node-node correspondence of the optimum assignment and the
	// cost of each node pairing.
	for (int left_node = 0; left_node < num_left_nodes; ++left_node) {
		const size_t right_idx = a.GetMate(left_node) - scores.size();
		assert(right_idx >= 0 && right_idx < scores.size());
		best_matching_out->operator[](left_node) = right_idx;
		summed_cost += scores.checked_at(left_node, right_idx);
	}
	double expected_deviation = std::nextafter(optimum_cost, HUGE_VALF) - optimum_cost;
	// assert(summed_cost == optimum_cost);
	BOOST_LOG_SEV(logger, lg::normal) << "Total score result by summing: " << summed_cost;
//	BOOST_LOG_SEV(logger, lg::normal) << "Total score result by OrTools directly: " << optimum_cost
//	                                  << " difference: " << std::abs(static_cast<double>(summed_cost - optimum_cost));

	return summed_cost;
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
		for (int to = 0; to < num_matches; ++to) {
			const int arc_tail = from; // must be in [0, num_left_nodes)
			const int arc_head = static_cast<int>(num_matches) + to; // must be in
			// [num_left_nodes,
			// num_nodes)
			graph.AddArc(arc_tail, arc_head);
		}
	}
	// Build the StaticGraph.
	graph.Build(&arc_permutation);
	BOOST_LOG_SEV(logger, lg::normal) << "Built static graph with " << num_nodes << " nodes.";
}
