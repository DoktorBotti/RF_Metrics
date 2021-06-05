#include "include/GeneralizedRfAlgo.h"
#include <boost/math/special_functions/factorials.hpp>
// boost logging
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>

GeneralizedRfAlgo::GeneralizedRfAlgo() {
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("generalized_RF"));
}

double GeneralizedRfAlgo::h_info_content(const PllSplit &S, size_t taxa, size_t split_len) {
	return (-1) * std::log(p_phy(S, taxa, split_len));
}

double GeneralizedRfAlgo::h_info_content(const PllSplit &S1,
                                         const PllSplit &S2,
                                         size_t taxa,
                                         size_t split_len) {
	return (-1) * std::log(p_phy(S1, S2, taxa, split_len));
}

double GeneralizedRfAlgo::h_info_content(const size_t a, const size_t b) {
	return (-1) * std::log(p_phy(a, b));
}

double inline GeneralizedRfAlgo::p_phy(const PllSplit &S, size_t taxa, size_t split_len) {
	const auto a = S.popcount(split_len);
	const auto b = taxa - a;

	return p_phy(a, b);
}

double inline GeneralizedRfAlgo::p_phy(const size_t a, const size_t b) {
	// if a or b < 1 then the double_factorial gets smaller than -1 which would be illegal
	assert(a >= 1);
	assert(b >= 1);
	return double_fac(2 * a - 3) *
	       double_fac(2 * b - 3) /
	       double_fac(2 * (a + b) - 5);
}

double inline GeneralizedRfAlgo::p_phy(const PllSplit &S1,
                                       const PllSplit &S2,
                                       size_t taxa,
                                       size_t split_len) {
	// TODO: ensure A2 \subseteq A1
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

	// TODO: Watch for numerical problems
	// TODO: implement explicit a!!/b!! method
	// TODO: vergleiche dass A1 > A2
	// TODO: Catch case a1 == a2
	// TODO: falsch... auch auf folien. man muss die kompatiblen segmente vergleichen
	return double_fac(2 * (b1 + 1) - 5) *
	       double_fac(2 * (a2 + 1) - 5) *
	       double_fac(2 * (a1 - a2 + 2) - 5) /
	       double_fac(2 * (taxa) -5);
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
	double total_dst = 0;

	// iterate through all tree combinations
	for (auto a = std::as_const(all_splits).begin(); a != all_splits.end(); ++a) {
		size_t idx_a = a - all_splits.begin();
		for (auto b = std::as_const(all_splits).begin(); b != a; ++b) {
			size_t idx_b = b - all_splits.begin();
			double dst = calc_tree_score(*a, *b);
			res.pairwise_distances_relative.set_at(idx_a, idx_b, dst);
			total_dst += dst;
		}
		double diag_dst = calc_tree_score(*a,*a);
		res.pairwise_distances_relative.set_at(idx_a,idx_a, diag_dst);
		total_dst += diag_dst;
	}
	// calc mean distance between trees
	res.mean_distance = total_dst / static_cast<double>(trees.size());
	return res;
}

double GeneralizedRfAlgo::calc_tree_score(const PllSplitList &A, const PllSplitList &B) {
	auto scores = calc_pairwise_split_scores(A, B);
	std::vector<size_t> mapping(scores.size(), -1);
	double total_score = match_solver.solve(scores, &mapping);
	std::stringstream out;

	for (size_t i = 0; i < mapping.size(); ++i) {
		out << " " << i << "<>" << mapping[i] << " ";
	}
	BOOST_LOG_SEV(logger, lg::notification) << "Mapping solution: " << out.str();
	return total_score;
}

RectMatrix<double> GeneralizedRfAlgo::calc_pairwise_split_scores(const PllSplitList &S1,
                                                                      const PllSplitList &S2) {
	RectMatrix<double> scores(S1.size());
	assert(S1.size() == S2.size());
	const auto taxa = S1.size() + 3;
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
double GeneralizedRfAlgo::double_fac(long x) {
	if (x >= 0) {
		try {
			return boost::math::double_factorial<double>(x);
		}catch (const std::exception& e){
			std::stringstream ss;
			ss << "Numerical overflow while calculating " << x << "!!\n";
            ss<< "Exception text: " << e.what();
			std::string str = ss.str();
			throw std::out_of_range(str.c_str());
		}
	} else if (x == -1) {
		return 1.;
	} else {
		throw std::invalid_argument("numbers less than -1 are not defined");
	}
}
