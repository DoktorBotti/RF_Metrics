#include "MatcherHungarian.h"
#include <hungarian.h>
#include <iostream>
Matcher::Scalar MatcherHungarian::solve(const RfAlgorithmInterface::SplitScores &scores) {
	const size_t dim = scores.scores.size();
	// create ideal multiplicator to create LAP in integer representation
	// the autor states, that two weights should be addable without overflow
	const int lap_factor =static_cast<int>( std::numeric_limits<int>::max() /2. / scores.max_score);
    auto getScore = [lap_factor, &scores](auto from, auto to){
		return static_cast<int>( -scores.scores.at(from,to) * lap_factor);
	};
	// create vector of bipartite edges on the order from -> all other edges
	std::vector<WeightedBipartiteEdge> lap_edges;
	lap_edges.reserve(dim * dim);
	for (size_t from = 0; from < dim; ++from) {
		for (size_t to = 0; to < dim; ++to) {
            lap_edges.emplace_back(static_cast<int>(from), static_cast<int>(to), getScore(from, to));
		}
	}
	auto edge_assignment_solution = hungarianMinimumWeightPerfectMatching(static_cast<int>(dim), std::move(lap_edges));

	assert(edge_assignment_solution.size() == dim);
	Scalar result = 0.;
	for (size_t from = 0; from < edge_assignment_solution.size(); ++from){
		auto to = static_cast<size_t>(edge_assignment_solution[from]);
		result += scores.scores.at(from,to);
	}
	return result;
}
