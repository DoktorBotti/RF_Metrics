#include "include/GeneralizedRfAlgo.h"
#include <boost/math/special_functions/factorials.hpp>
#include <queue>
// boost logging
#include <FastSplitList.h>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>

LogDblFact GeneralizedRfAlgo::factorials = LogDblFact();
GeneralizedRfAlgo::GeneralizedRfAlgo() {
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("generalized_RF"));
}

// TODO: optimize h_info_contents and remove split_len from params

RfAlgorithmInterface::Scalar
GeneralizedRfAlgo::h_info_content(const PllSplit &S, size_t taxa, size_t split_len) {
	size_t a = S.pre_calc_popcount;
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

RfAlgorithmInterface::Scalar GeneralizedRfAlgo::h_info_content(size_t a, size_t b) {
	return -p_phy(a, b);
}

RfAlgorithmInterface::Scalar inline GeneralizedRfAlgo::p_phy(const PllSplit &S,
                                                             size_t taxa,
                                                             size_t split_len) {
	auto a = S.pre_calc_popcount;
	auto b = taxa - a;

	return p_phy(a, b);
}

RfAlgorithmInterface::Scalar GeneralizedRfAlgo::p_phy(size_t a, size_t b) {
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
	// TODO: reuse vector to stop calling malloc/free
	std::vector<pll_split_base_t> cut(split_len);
	S1.intersect(S2, split_len, &cut[0]);

	size_t a1, b1, a2;

	if (S2.equals(PllSplit(&cut[0], split_len), split_len)) {
		a1 = S1.pre_calc_popcount;
		b1 = taxa - a1;
		a2 = S2.pre_calc_popcount;
	} else if (S1.equals(PllSplit(&cut[0], split_len), split_len)) {
		a1 = S2.pre_calc_popcount;
		b1 = taxa - a1;
		a2 = S1.pre_calc_popcount;
	} else {
		a1 = S1.pre_calc_popcount;
		b1 = taxa - a1;
		auto b2 = S2.pre_calc_popcount;
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
	PllSplit::split_len = all_splits.back().computeSplitLen();

	BOOST_LOG_SEV(logger, lg::notification) << "Parsed trees. Starting calculations.";
	std::vector<FastSplitList> fast_trees = generateFastList(all_splits);
	assert(PllSplit::split_len != std::numeric_limits<size_t>::max());
	setup_temporary_storage(PllSplit::split_len);
	RfMetricInterface::Results res(trees.size());
	Scalar total_dst = 0;
	size_t tree_num = 0;
	size_t num_tree_calcs = all_splits.size() * (all_splits.size() + 1) / 2;

	// iterate through all tree combinations
	for (size_t idx_a = 0; idx_a < all_splits.size(); ++idx_a) {
		for (size_t idx_b = 0; idx_b <= idx_a; ++idx_b) {
			Scalar dst = calc_tree_score(fast_trees[idx_a], fast_trees[idx_b]);
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
RfAlgorithmInterface::Scalar GeneralizedRfAlgo::calc_tree_score(const SplitList &A,
                                                                const SplitList &B) {
	auto scores = calc_pairwise_split_scores(A, B);
	Scalar total_score = match_solver.solve(scores);
	//	std::stringstream out;
	//	for (size_t i = 0; i < mapping.size(); ++i) {
	//	for (size_t i = 0; i < mapping.size(); ++i) {
	//		out << " " << i << "<>" << mapping[i] << " ";
	//	}
	//	BOOST_LOG_SEV(logger, lg::notification) << "Mapping solution: " << out.str();
	return total_score;
}

GeneralizedRfAlgo::SplitScores GeneralizedRfAlgo::calc_pairwise_split_scores(const SplitList &S1,
                                                                             const SplitList &S2) {
	SplitScores scores(S1.size());
	const auto taxa = S1.size() + 3;
	factorials.reserve(taxa + taxa);
	const auto split_len = S1.computeSplitLen();
	for (size_t row = 0; row < S1.size(); ++row) {
		for (size_t col = 0; col < S1.size(); ++col) {
            Scalar val;
			// when using fast Split list, the pointers to PllSplit define equality
			if(&S1[row] == &S2[col]){
				assert(S1[row] == S2[col]);
				val = calc_split_score(S1[row], taxa, split_len);
			}else{
                val = calc_split_score(S1[row], S2[col], taxa, split_len);
            }

			if (scores.max_score < val) {
				scores.max_score = val;
			}
			scores.scores.set(row, col, val);
		}
	}

	return scores;
}

void GeneralizedRfAlgo::compute_split_comparison(const PllSplit &S1,
                                                 const PllSplit &S2,
                                                 size_t split_len) {
	// B1 -> &split_buffer[0]
	S1.set_not(split_len, &temporary_split_content[0]);
	// B2 -> &split_buffer[split_len]
	S2.set_not(split_len, &temporary_split_content[split_len]);
	// A1 and A2 -> &split_buffer[2 * split_len]
	S1.intersect(S2, split_len, &temporary_split_content[2 * split_len]);
	// B1 and B2 -> &split_buffer[3 * split_len]
	temporary_splits[0].intersect(
	    temporary_splits[1], split_len, &temporary_split_content[3 * split_len]);
	// A1 and B2 -> &split_buffer[4 * split_len]
	S1.intersect(temporary_splits[1], split_len, &temporary_split_content[4 * split_len]);
	// A2 and B1 -> &split_buffer[5 * split_len]
	S2.intersect(temporary_splits[0], split_len, &temporary_split_content[5 * split_len]);
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
void GeneralizedRfAlgo::setup_temporary_storage(size_t split_len) {
	temporary_split_content.assign(split_len * 6, 0);
	temporary_splits.reserve(6);
	for (size_t i = 0; i < 6; ++i) {
		temporary_splits.emplace_back(&temporary_split_content[i * split_len]);
	}
}
GeneralizedRfAlgo::GeneralizedRfAlgo(size_t split_len) {
    logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("generalized_RF"));
	setup_temporary_storage(split_len);
}
std::vector<FastSplitList>
GeneralizedRfAlgo::generateFastList(const std::vector<PllSplitList> &slow_split_list) {
	// expect that all elements of slow_split_list[i] are already sorted
	// -> Perform k-way merge to only store non-duplicate PllSplits
	PllSplit::split_len = slow_split_list.front().computeSplitLen();
	BOOST_LOG_SEV(logger, lg::normal)
	    << "Start reducing PllSplit Size, current split_len: " << PllSplit::split_len;

	// rough estimate on how many PllSplit entries there are
	const auto reserve_size = static_cast<size_t>(slow_split_list.size() * slow_split_list.front().size() / 1.5);
	unique_pll_splits.reserve(reserve_size);
	std::vector<FastSplitList> returnList(slow_split_list.size(),
	                                      FastSplitList(slow_split_list.front().size()));

	// data used inside k-way merge
	std::vector<size_t> currently_inPQ(slow_split_list.size(), 0);
	size_t current_split_offset = 0;
	size_t found_duplicates = 0;
	// PQ which stores a PllSplit (underlying data not touched) and the index from which tree it
	// originates from
	typedef std::pair<PllSplit, size_t> pq_type;
	std::priority_queue<pq_type, std::vector<pq_type>, std::greater<>> pq;
	// initialize by inserting first elements of all slow lists
	for (size_t i = 0; i < slow_split_list.size(); ++i) {
		pq.push(std::make_pair(PllSplit(slow_split_list[i][0]), i));
	}
	auto insert_in_pq = [&](size_t active_slow_idx) {
		// takes the current index
		auto &active_slow_list = slow_split_list[active_slow_idx];
		// increment where the active element is
		size_t list_idx = ++currently_inPQ[active_slow_idx];
		if (list_idx < active_slow_list.size()) {
			PllSplit insert_split(active_slow_list[list_idx]());
			pq.push(std::make_pair(insert_split, active_slow_idx));
		}
	};
	while (!pq.empty()) {
		// no further duplications store the next split in unique_pll_splits (either because of
		// start/or handled all duplicates), increment current_split_offset
		unique_pll_splits.emplace_back(pq.top().first);
		unique_pll_splits.back().perform_popcount_precalc(PllSplit::split_len);
		// get current state
		PllSplit curr_split = pq.top().first;
		{
			size_t curr_slow_list_idx = pq.top().second;
			// add current unique split to result
			assert(currently_inPQ[curr_slow_list_idx] < slow_split_list.back().size());
			returnList[curr_slow_list_idx].setOffsetAt(currently_inPQ[curr_slow_list_idx],
			                                           current_split_offset);
			// add from the same list a replacement element
			pq.pop();

			insert_in_pq(curr_slow_list_idx);
		}
		// check if further elements are equal
		while (!pq.empty() && curr_split == pq.top().first) {
			++found_duplicates;
			// duplicate found - use the same offset value
			size_t equal_in_tree_idx = pq.top().second;
			// the found value must have un-inserted elements
			assert(currently_inPQ[equal_in_tree_idx] < slow_split_list[equal_in_tree_idx].size());
			returnList[equal_in_tree_idx].setOffsetAt(currently_inPQ[equal_in_tree_idx],
			                                          current_split_offset);
			pq.pop();
			// insertion of replacement for equal_in_tree element
			insert_in_pq(equal_in_tree_idx);
		}
        ++current_split_offset;
    }
	{
		size_t total_splits = slow_split_list.size() * slow_split_list.front().size();
		double duplicate_ratio =
		    static_cast<double>(found_duplicates) / static_cast<double>(total_splits);
		BOOST_LOG_SEV(logger, lg::notification)
		    << "Done construction of FastSplitList. " << found_duplicates << " duplicates of "
		    << total_splits << " possible PllSplits. Duplicate ratio: " << duplicate_ratio;
		BOOST_LOG_SEV(logger, lg::notification)
		    << "Estimated " << reserve_size
		    << " unique pll splits, actual number: " << unique_pll_splits.size();
	}
	// unique_pll_splits will no longer reallocate -> write base-ptr to Static variable
	FastSplitList::setBasePtr(&unique_pll_splits[0]);
	return returnList;
}
