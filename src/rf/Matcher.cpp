#include "Matcher.h"
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>

Matcher::Scalar Matcher::solve(const RectMatrix<Matcher::Scalar> &scores
                               /*,std::vector<size_t> *best_matching_out*/) {
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
	parameterize_assignment(scores, a);

	// Compute the optimum assignment.
	BOOST_LOG_SEV(logger, lg::normal) << "Calculating assignment";
	if(!a.FinalizeSetup()){
		throw std::logic_error("The Linear Assignment Calculator cannot guarantee a valid result");
	}
	bool success = a.ComputeAssignment();
	assert(success);
	if (success) {
		BOOST_LOG_SEV(logger, lg::normal) << "Finished assignment sucessfully.";
	} else {
		BOOST_LOG_SEV(logger, lg::error) << "Finished assignment with errors.";
	}
	// Retrieve the cost of the optimum assignment.
	Matcher::Scalar optimum_cost =
	    static_cast<double>(-a.GetCost()) / static_cast<double>(large_num);
	//	Matcher::Scalar summed_cost = 0;
	//	// Retrieve the node-node correspondence of the optimum assignment and the
	//	// cost of each node pairing.
	//	for (int left_node = 0; left_node < num_left_nodes; ++left_node) {
	//		const size_t right_idx = static_cast<size_t>(a.GetMate(left_node)) - scores.size();
	//		assert(right_idx >= 0 && right_idx < scores.size());
	//		best_matching_out->operator[](static_cast<size_t>(left_node)) = right_idx;
	//		Matcher::Scalar arc_score = scores.at(static_cast<size_t>(left_node), right_idx);
	//		{
	//			// DEBUG verify that chosen arc index is left_id * size + right_id - size
	//			auto arc_mate_id = a.GetAssignmentArc(left_node);
	//			// Softwipe..
	//			int arc_should_id = static_cast<int>(static_cast<size_t>(left_node) * scores.size()
	//+ right_idx); 			assert(arc_mate_id == arc_should_id);
	//			// DEBUG if arc indices were as they should be
	//			auto arc_score_diff = std::abs(arc_score + static_cast<Matcher::Scalar>(
	// a.GetAssignmentCost(left_node)) / static_cast<Matcher::Scalar>( large_num));
	//			assert(arc_score_diff < 1e-6);
	//		}
	//		summed_cost += arc_score;
	//	}
	BOOST_LOG_SEV(logger, lg::normal) << "Total score result by summing: " << optimum_cost;
	//	BOOST_LOG_SEV(logger, lg::normal) << "Total score result by OrTools directly: " <<
	// optimum_cost
	//	                                  << " difference: " <<
	// std::abs(static_cast<double>(summed_cost - optimum_cost));

	return optimum_cost;
}
operations_research::LinearSumAssignment<Matcher::Graph> &Matcher::parameterize_assignment(
    const RectMatrix<Matcher::Scalar> &scores,
    operations_research::LinearSumAssignment<Matcher::Graph> &a) { // tiny lambda helpers
	auto getScore = [scores](size_t i, size_t j) -> long {
		// multiply score with a high value to make rounding errors less troublesome
		Matcher::Scalar score = -scores.at(i, j) * static_cast<double>(large_num);
		return static_cast<long>(score);
	};
	for (size_t from = 0; from < scores.size(); ++from) {
		for (size_t to = 0; to < scores.size(); ++to) {
			// TODO problem that casted to long?
			long arc_cost = getScore(from, to);
			// from -> to
			assign_permuted_cost(from * scores.size() + to, arc_cost, a);
		}
	}
	return a;
}
Matcher::Matcher() {
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("Matcher"));
}
void Matcher::assign_permuted_cost(size_t unpermuted_index,
                                   long cost,
                                   operations_research::LinearSumAssignment<Graph> &assignment) {
	int idx = static_cast<int>(unpermuted_index);
	int perm_id = arc_permutation.empty() ? idx : arc_permutation[static_cast<size_t>(idx)];
	assignment.SetArcCost(perm_id, cost);
}
void Matcher::init(size_t num_matches) {
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
void Matcher::debugAssignment(const RectMatrix<Matcher::Scalar> &scores,
                              operations_research::LinearSumAssignment<Matcher::Graph> *out) {
	using namespace operations_research;
	// initialize on first usage
	if (!is_ready) {
		init(scores.size());
		is_ready = true;
	}
	parameterize_assignment(scores, *out);
}
Matcher::Graph Matcher::getGraphCopy(const RectMatrix<Matcher::Scalar> &scores) {
	using namespace operations_research;
	// initialize on first usage
	if (!is_ready) {
		init(scores.size());
		is_ready = true;
	}
	return graph;
}
