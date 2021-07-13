#include "MatcherOrTools.h"
#include <GeneralizedRfAlgo.h>
#include <boost/log/sources/record_ostream.hpp>
#include <climits>

std::future<MatcherOrTools::Scalar>
MatcherOrTools::solve(GeneralizedRfAlgo::SplitScores &scores
                      /*,std::vector<size_t> *best_matching_out*/) {
	// initialize on first usage
	if (!is_ready) {
		init(scores.scores.size());
		is_ready = true;
	}
	//
	//	BOOST_LOG_SEV(logger, lg::normal) << "Total score result by summing: " << optimum_cost;
	//	BOOST_LOG_SEV(logger, lg::normal)
	//	    << "Total score result by OrTools directly: " << optimum_cost
	//	    << " difference: " << std::abs(static_cast<double>(summed_cost - optimum_cost));

	return std::async(
	    [](auto scores, const auto &graph_instance) {
		    return parallel_calc(scores, graph_instance);
	    },
	    scores,
	    graph);
}
operations_research::LinearSumAssignment<MatcherOrTools::Graph> &
MatcherOrTools::parameterize_assignment(
    const MatcherOrTools::Graph &graph,
    const RfAlgorithmInterface::SplitScores &scores,
    operations_research::LinearSumAssignment<MatcherOrTools::Graph> &a,
    const double lap_factor) { // tiny lambda helpers
	// auto lap_factor = static_cast<double>(LLONG_MAX / scores.scores.size()) / scores.max_score;
	auto getScore = [scores, lap_factor](size_t i, size_t j) -> long {
		// multiply score with a high value to make rounding errors less troublesome
		MatcherOrTools::Scalar score = -scores.scores.at(i, j) * lap_factor;
		return static_cast<long>(score);
	};
	const auto dim = scores.scores.size();
	for (size_t from = 0; from < dim; ++from) {
		for (size_t to = 0; to < dim; ++to) {
			const long arc_cost = getScore(from, to);
			const auto arc_idx = from * dim + to;
			assign_permuted_cost(arc_idx, arc_cost, a, std::vector<int>());
			// Debug:
			assert(graph.Tail(arc_idx) == static_cast<int>(from));
			assert(graph.Head(arc_idx) == static_cast<int>(to + dim));
		}
	}
	return a;
}
MatcherOrTools::MatcherOrTools() {
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("Matcher"));
}
void MatcherOrTools::assign_permuted_cost(
    size_t unpermuted_index,
    long cost,
    operations_research::LinearSumAssignment<Graph> &assignment,
    const std::vector<int> &arc_permutations) {
	int idx = static_cast<int>(unpermuted_index);
	int perm_id = arc_permutations.empty() ? idx : arc_permutations[static_cast<size_t>(idx)];
	assignment.SetArcCost(perm_id, cost);
}
void MatcherOrTools::init(size_t num_matches) {
	using namespace operations_research;
	BOOST_LOG_SEV(logger, lg::normal) << "Initializing...";

	const size_t num_nodes = 2 * num_matches;
	const size_t num_arcs = num_matches * num_matches;
	graph = Graph(static_cast<int>(num_nodes), static_cast<int>(num_arcs));
	// create edges
	for (int from = 0; from < static_cast<int>(num_matches); ++from) {
		for (int to = static_cast<int>(num_matches); to < static_cast<int>(num_nodes); ++to) {
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
MatcherOrTools::Graph
MatcherOrTools::getGraphCopy(const RectMatrix<MatcherOrTools::Scalar> &scores) {
	using namespace operations_research;
	// initialize on first usage
	if (!is_ready) {
		init(scores.size());
		is_ready = true;
	}
	return graph;
}
double MatcherOrTools::parallel_calc(RfAlgorithmInterface::SplitScores &scores,
                                     const MatcherOrTools::Graph &graph) {
	// performance accuracy trade-off
	auto lap_factor = static_cast<double>(LLONG_MAX) /
	                  static_cast<double>(10000 * scores.scores.size()) /
	                  (scores.max_score > 1. ? scores.max_score : 1.);

	// find maximal pairwise score, will be needed since the solver minimizes cost
	using namespace operations_research;
	// Construct the LinearSumAssignment.
	int num_left_nodes = static_cast<int>(scores.scores.size());
	LinearSumAssignment<Graph> a(graph, num_left_nodes);
	parameterize_assignment(graph, scores, a, lap_factor);

	// Compute the optimum assignment.
	if (!a.FinalizeSetup()) {
		throw std::logic_error("The Linear Assignment Calculator cannot guarantee a valid result");
	}
	bool success = a.ComputeAssignment();
	assert(success);

	// Retrieve the cost of the optimum assignment.
	MatcherOrTools::Scalar summed_cost = 0;
	// Retrieve the node-node correspondence of the optimum assignment and the
	// cost of each node pairing.
	for (int left_node = 0; left_node < num_left_nodes; ++left_node) {
		const size_t right_idx = static_cast<size_t>(a.GetMate(left_node)) - scores.scores.size();
		assert(right_idx < scores.scores.size());
		//			best_matching_out->operator[](static_cast<size_t>(left_node)) = right_idx;
		MatcherOrTools::Scalar arc_score =
		    scores.scores.at(static_cast<size_t>(left_node), right_idx);
		summed_cost += arc_score;
	}
	return summed_cost;
}
