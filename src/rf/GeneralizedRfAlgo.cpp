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
	RfMetricInterface::Results res(trees.size());
	Scalar total_dst = 0;

	// iterate through all tree combinations
	for (size_t idx_a = 0; idx_a < all_splits.size(); ++idx_a) {
		for (size_t idx_b = 0; idx_b <= idx_a; ++idx_b) {
			Scalar dst = calc_tree_score(all_splits[idx_a], all_splits[idx_b]);
			res.pairwise_similarities.set_at(idx_a, idx_b, dst);
			total_dst += dst;
		}
	}
	// calc mean distance between trees
	res.mean_distance = total_dst / static_cast<Scalar>(trees.size());
	return res;
}

RfAlgorithmInterface::Scalar GeneralizedRfAlgo::calc_tree_score(const PllSplitList &A,
                                                                const PllSplitList &B) {
	auto scores = calc_pairwise_split_scores(A, B);
	Scalar total_score = match_solver.solve(scores);
	std::stringstream out;

	//	for (size_t i = 0; i < mapping.size(); ++i) {
	//		out << " " << i << "<>" << mapping[i] << " ";
	//	}
	//	BOOST_LOG_SEV(logger, lg::notification) << "Mapping solution: " << out.str();
	return total_score;
}

RectMatrix<RfAlgorithmInterface::Scalar>
GeneralizedRfAlgo::calc_pairwise_split_scores(const PllSplitList &S1, const PllSplitList &S2) {
	RectMatrix<Scalar> scores(S1.size());
    const auto taxa = S1.size() + 3;
    factorials.reserve(taxa);
    const auto split_len = S1.computeSplitLen();
	for (size_t row = 0; row < S1.size(); ++row) {
		for (size_t col = 0; col < S1.size(); ++col) {
			scores.set(row, col, calc_split_score(S1[row], S2[col], taxa, split_len));
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
