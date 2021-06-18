#include "include/GeneralizedRfAlgo.h"
#include <boost/math/special_functions/factorials.hpp>
// boost logging
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>

LogDblFact GeneralizedRfAlgo::factorials = LogDblFact();
GeneralizedRfAlgo::GeneralizedRfAlgo() {
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("generalized_RF"));
}

RfAlgorithmInterface::Scalar
GeneralizedRfAlgo::h_info_content(const PllSplit &S, size_t taxa, size_t split_len) {
	size_t a = S.popcount(split_len);
	size_t b = taxa - a;

	Scalar res = factorials.lg_rooted_dbl_fact_fast(static_cast<long>(a)) +
	             factorials.lg_rooted_dbl_fact_fast(static_cast<long>(b)) -
	             factorials.lg_unrooted_dbl_fact_fast(static_cast<long>(taxa));

	return -res;
}

RfAlgorithmInterface::Scalar GeneralizedRfAlgo::h_info_content(const PllSplit &S1,
                                                               const PllSplit &S2,
                                                               size_t taxa,
                                                               size_t split_len) {
	return -p_phy(S1, S2, taxa, split_len);
}

RfAlgorithmInterface::Scalar GeneralizedRfAlgo::h_info_content(const size_t a, const size_t b) {
	return -p_phy(a, b);
}

RfAlgorithmInterface::Scalar inline GeneralizedRfAlgo::p_phy(const PllSplit &S,
                                                             size_t taxa,
                                                             size_t split_len) {
	const auto a = S.popcount(split_len);
	const auto b = taxa - a;

	return p_phy(a, b);
}

RfAlgorithmInterface::Scalar inline GeneralizedRfAlgo::p_phy(const size_t a, const size_t b) {
	// no trivial splits allowed here (outer log would return infty, because no information present)
	assert(a >= 2);
	assert(b >= 2);
	// precompute (maybe lazy) all double fac results for a and b separatly
	return factorials.lg_rooted_dbl_fact_fast(static_cast<long>(a)) +
	       factorials.lg_rooted_dbl_fact_fast(static_cast<long>(b)) -
	       factorials.lg_unrooted_dbl_fact_fast(static_cast<long>(a + b));
}

RfAlgorithmInterface::Scalar inline GeneralizedRfAlgo::p_phy(const PllSplit &S1,
                                                             const PllSplit &S2,
                                                             size_t taxa,
                                                             size_t split_len) {
	std::vector<pll_split_base_t> cut(split_len);
	S1.intersect(S2, split_len, &cut[0]);

	size_t a1, b1, a2;

	if (S2.equals(PllSplit(&cut[0]), split_len)) {
		a1 = S1.popcount(split_len);
		b1 = taxa - a1;
		a2 = S2.popcount(split_len);
	} else if (S1.equals(PllSplit(&cut[0]), split_len)) {
		a1 = S2.popcount(split_len);
		b1 = taxa - a1;
		a2 = S1.popcount(split_len);
	} else {
		a1 = S1.popcount(split_len);
		b1 = taxa - a1;
		auto b2 = S2.popcount(split_len);
		a2 = taxa - b2;
	}

	if (a1 == a2)
		return p_phy(a1, b1);

	// assertions so that the double factorials are defined
	assert(a1 >= a2);
	assert(b1 >= 1);
	assert(a2 >= 1);
	assert(taxa >= 2);

	return factorials.lg_rooted_dbl_fact_fast(static_cast<long>(b1)) +
	       factorials.lg_rooted_dbl_fact_fast(static_cast<long>(a2)) +
	       factorials.lg_dbl_fact_fast(static_cast<long>(((a1 - a2) << 1) - 1)) -
	       factorials.lg_unrooted_dbl_fact_fast(static_cast<long>(taxa));
}

size_t GeneralizedRfAlgo::bits_too_many(size_t taxa) {
	constexpr size_t bit_amount_split = sizeof(pll_split_base_t) * 8;
	auto bits_too_many =
	    taxa % bit_amount_split == 0 ? 0 : bit_amount_split - (taxa % bit_amount_split);
	assert(bits_too_many < sizeof(pll_split_base_t) * 8);
	return bits_too_many;
}

RfMetricInterface::Results GeneralizedRfAlgo::calculate(std::vector<PllTree> &trees) {
	assert(trees.size() >= 2);
	// extract splits. Each tree now identifies by its index in all_splits
	std::vector<PllSplitList> all_splits;
	all_splits.reserve(trees.size());
	for (auto &t : trees) {
		t.alignNodeIndices(*trees.begin());
		all_splits.emplace_back(t);
	}
	BOOST_LOG_SEV(logger, lg::notification) << "Parsed trees. Starting calculations.";
	RfMetricInterface::Results res(trees.size());
	Scalar total_dst = 0;
	size_t tree_num = 0;
	const size_t num_tree_calcs = all_splits.size() * (all_splits.size() + 1) / 2;

	// iterate through all tree combinations
	for (size_t idx_a = 0; idx_a < all_splits.size(); ++idx_a) {
		for (size_t idx_b = 0; idx_b <= idx_a; ++idx_b) {
			Scalar dst = calc_tree_score(all_splits[idx_a], all_splits[idx_b]);
			res.pairwise_similarities.set_at(idx_a, idx_b, dst);
			total_dst += dst;
			++tree_num;
			if (tree_num % 50 == 0) {
				// print update
				BOOST_LOG_SEV(logger, lg::notification)
				    << "Processed " << tree_num << " of " << num_tree_calcs
				    << " total tree calculations";
			}
		}
	}

	// calc mean distance between trees
	res.mean_distance = total_dst / static_cast<Scalar>(trees.size());

	// calculate distances
	calc_pairwise_tree_dist(all_splits, res);
	return res;
}

RfAlgorithmInterface::Scalar GeneralizedRfAlgo::calc_tree_score(const PllSplitList &A,
                                                                const PllSplitList &B) {
	auto scores = calc_pairwise_split_scores(A, B);
	Scalar total_score = match_solver.solve(scores);
	// std::stringstream out;
	//	for (size_t i = 0; i < mapping.size(); ++i) {
	//		out << " " << i << "<>" << mapping[i] << " ";
	//	}
	//	BOOST_LOG_SEV(logger, lg::notification) << "Mapping solution: " << out.str();
	return total_score;
}

GeneralizedRfAlgo::SplitScores
GeneralizedRfAlgo::calc_pairwise_split_scores(const PllSplitList &S1, const PllSplitList &S2) {
	SplitScores scores(S1.size());
	const auto taxa = S1.size() + 3;
	factorials.reserve(taxa + taxa);
	const auto split_len = S1.computeSplitLen();
	for (size_t row = 0; row < S1.size(); ++row) {
		for (size_t col = 0; col < S1.size(); ++col) {
			auto val = calc_split_score(S1[row], S2[col], taxa, split_len);
			if (scores.max_score < val) {
				scores.max_score = val;
			}
			scores.scores.set(row, col, val);
		}
	}

	return scores;
}

std::vector<pll_split_base_t> GeneralizedRfAlgo::compute_split_comparison(const PllSplit &S1,
                                                                          const PllSplit &S2,
                                                                          size_t split_len) {
	// TODO: is there something more lightweight (like an array?)
	std::vector<pll_split_base_t> split_buffer(split_len * 6);
	// B1 -> &split_buffer[0]
	S1.set_not(split_len, &split_buffer[0]);
	// B2 -> &split_buffer[split_len]
	S2.set_not(split_len, &split_buffer[split_len]);
	// A1 and A2 -> &split_buffer[2 * split_len]
	S1.intersect(S2, split_len, &split_buffer[2 * split_len]);
	// B1 and B2 -> &split_buffer[3 * split_len]
	PllSplit(&split_buffer[0])
	    .intersect(PllSplit(&split_buffer[split_len]), split_len, &split_buffer[3 * split_len]);
	// A1 and B2 -> &split_buffer[4 * split_len]
	S1.intersect(PllSplit(&split_buffer[split_len]), split_len, &split_buffer[4 * split_len]);
	// A2 and B1 -> &split_buffer[5 * split_len]
	S2.intersect(PllSplit(&split_buffer[0]), split_len, &split_buffer[5 * split_len]);

	return split_buffer;
}
RfAlgorithmInterface::Scalar
GeneralizedRfAlgo::calc_tree_info_content(const PllSplitList &S, size_t taxa, size_t split_len) {
	Scalar sum = 0;
	for (size_t i = 0; i < S.size(); ++i) {
		sum += h_info_content(S[i], taxa, split_len);
	}
	return sum;
}
void GeneralizedRfAlgo::calc_pairwise_tree_dist(const std::vector<PllSplitList> &trees,
                                                RfMetricInterface::Results &res) {
	std::vector<GeneralizedRfAlgo::Scalar> tree_info(trees.size());
	auto taxa = trees[0].size() + 3;
	auto split_len = trees[0].computeSplitLen();
	for (size_t i = 0; i < trees.size(); ++i) {
		tree_info[i] = calc_tree_info_content(trees[i], taxa, split_len);
	}

	GeneralizedRfAlgo::Scalar summed_dist = 0;
	for (size_t row = 0; row < trees.size(); ++row) {
		for (size_t col = 0; col <= row; ++col) {
			auto score = res.pairwise_similarities.at(row, col);
			auto max = tree_info[row] + tree_info[col];
			res.pairwise_distances.set_at(row, col, (max - score - score));
			summed_dist += max / score;
		}
	}

	res.mean_distance = summed_dist / (static_cast<Scalar>(trees.size() + 1) *
	                                   (static_cast<Scalar>(trees.size()) / 2.));
}
