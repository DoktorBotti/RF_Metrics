#include "include/GeneralizedRfAlgo.h"
#include <boost/math/special_functions/factorials.hpp>
#include <queue>
// boost logging
#include <FastSplitList.h>
#include <SpiAlgo.h>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <future>

std::atomic_size_t GeneralizedRfAlgo::tree_idx = 0;

LogDblFact GeneralizedRfAlgo::factorials = LogDblFact();
GeneralizedRfAlgo::GeneralizedRfAlgo() : pairwise_split_scores(0) {
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("generalized_RF"));
}

RfAlgorithmInterface::Scalar GeneralizedRfAlgo::h_info_content(size_t a, size_t b) {
	// no trivial splits allowed here (outer log would return infty, because no information present)
	assert(a >= 2);
	assert(b >= 2);
	// precompute (maybe lazy) all double fac results for a and b separatly
	return -factorials.lg_rooted_dbl_fact_fast(static_cast<long>(a)) -
	       factorials.lg_rooted_dbl_fact_fast(static_cast<long>(b)) +
	       factorials.lg_unrooted_dbl_fact_fast(static_cast<long>(a + b));
}

size_t GeneralizedRfAlgo::bits_too_many(size_t taxa) {
	constexpr size_t bit_amount_split = sizeof(pll_split_base_t) * 8;
	auto bits_too_many =
	    taxa % bit_amount_split == 0 ? 0 : bit_amount_split - (taxa % bit_amount_split);
	assert(bits_too_many < sizeof(pll_split_base_t) * 8);
	return bits_too_many;
}

void GeneralizedRfAlgo::calc_thread(GeneralizedRfAlgo &alg,
                                    const std::vector<FastSplitList> &trees,
                                    size_t pairwise_tree_cnt,
                                    SymmetricMatrix<Scalar> &sim) {
	while (true) {
		size_t index = tree_idx++;
		if (index >= pairwise_tree_cnt) {
			break;
		}
		// Calculates the corresponding row from the index using the quadratic formula
		auto row = static_cast<size_t>(std::sqrt(1 + 8 * index) / 2 - .5);
		size_t col = index - (row * row + row) / 2;
		auto score = alg.calc_tree_score(trees[row], trees[col]);
		sim.raw_set_at(index, score);
	}
}

RfMetricInterface::Results GeneralizedRfAlgo::calculate(std::vector<PllTree> &trees,
                                                        const RfMetricInterface::Params &params) {
	assert(trees.size() >= 2);
	// extract splits. Each tree now identifies by its index in all_splits
	std::vector<PllSplitList> all_splits;
	all_splits.reserve(trees.size());
	for (auto &t : trees) {
		t.alignNodeIndices(*trees.begin());
		all_splits.emplace_back(t);
	}
	PllSplit::split_len = all_splits.back().computeSplitLen();
	taxa = all_splits.back().size() + 3;

	BOOST_LOG_SEV(logger, lg::notification) << "Parsed trees. Starting calculations.";
	factorials.reserve(4 * taxa + 8);
	std::vector<FastSplitList> fast_trees = generateFastList(all_splits);
	assert(PllSplit::split_len != std::numeric_limits<size_t>::max());
	setup_temporary_storage(PllSplit::split_len);
	pairwise_split_scores = calcPairwiseSplitScores();

	BOOST_LOG_SEV(logger, lg::notification)
	    << "Calculated pairwise scores; Calculating pairwise tree scores.";
	RfMetricInterface::Results res(trees.size());

	size_t pairwise_tree_cnt = res.pairwise_similarities.get_num_entries();
	int num_threads = params.threads == -1 ? static_cast<int>(std::thread::hardware_concurrency())
	                                       : params.threads;
	std::vector<std::thread> pool;
	match_solver.init(all_splits.back().size());

	pool.reserve(num_threads);
    for (int i = 0; i < num_threads; ++i) {
		pool.emplace_back(calc_thread,
		                  std::ref(*this),
		                  std::ref(fast_trees),
		                  std::ref(pairwise_tree_cnt),
		                  std::ref(res.pairwise_similarities));
	}

	for (auto &thread : pool) {
		thread.join();
	}

	// calc mean distance between trees
	double total_dst = 0.;
	for (size_t idx_a = 0; idx_a < all_splits.size(); ++idx_a) {
		for (size_t idx_b = 0; idx_b <= idx_a; ++idx_b) {
			total_dst += res.pairwise_similarities.at(idx_a, idx_b);
		}
	}
	res.mean_distance = total_dst / static_cast<Scalar>(trees.size());

	// calculate distances
	calc_pairwise_tree_dist(fast_trees, res);

	factorials.printLog();
	return res;
}
RfAlgorithmInterface::Scalar GeneralizedRfAlgo::calc_tree_score(const SplitList &A,
                                                                const SplitList &B) {
	//	auto scores = calc_pairwise_split_scores(A, B);
	SplitScores scores(A.size());
	Scalar max_val = 0;
	for (size_t row = 0; row < A.size(); ++row) {
		size_t row_idx = A[row].getScoreIndex();
		for (size_t col = 0; col < A.size(); ++col) {
			size_t col_idx = B[col].getScoreIndex();
			auto val = pairwise_split_scores.checked_at(row_idx, col_idx);
			scores.scores.set(row, col, val);
			if (val > max_val) {
				max_val = val;
			}
		}
	}
	scores.max_score = max_val;
	auto total_score = match_solver.solve(scores);
	return total_score;
}

[[maybe_unused]] GeneralizedRfAlgo::SplitScores
GeneralizedRfAlgo::calc_pairwise_split_scores(const SplitList &S1, const SplitList &S2) {
	SplitScores scores(S1.size());
	factorials.reserve(taxa + taxa + 4);
	for (size_t row = 0; row < S1.size(); ++row) {
		for (size_t col = 0; col < S1.size(); ++col) {
			Scalar val;
			// when using fast Split list, the pointers to PllSplit define equality
			if (&S1[row] == &S2[col]) {
				assert(S1[row] == S2[col]);
				val = calc_split_score(S1[row]);
			} else {
				val = calc_split_score(S1[row], S2[col]);
			}

			if (scores.max_score < val) {
				scores.max_score = val;
			}
			scores.scores.set(row, col, val);
		}
	}

	return scores;
}

void GeneralizedRfAlgo::compute_split_comparison(const PllSplit &S1, const PllSplit &S2) {
	// B1 -> &split_buffer[0]
	S1.set_not(PllSplit::split_len, &temporary_split_content[0]);
	// B2 -> &split_buffer[split_len]
	S2.set_not(PllSplit::split_len, &temporary_split_content[PllSplit::split_len]);
	// A1 and A2 -> &split_buffer[2 * split_len]
	S1.intersect(S2, PllSplit::split_len, &temporary_split_content[2 * PllSplit::split_len]);
	// B1 and B2 -> &split_buffer[3 * split_len]
	temporary_splits[0].intersect(temporary_splits[1],
	                              PllSplit::split_len,
	                              &temporary_split_content[3 * PllSplit::split_len]);
	// A1 and B2 -> &split_buffer[4 * split_len]
	S1.intersect(temporary_splits[1],
	             PllSplit::split_len,
	             &temporary_split_content[4 * PllSplit::split_len]);
	// A2 and B1 -> &split_buffer[5 * split_len]
	S2.intersect(temporary_splits[0],
	             PllSplit::split_len,
	             &temporary_split_content[5 * PllSplit::split_len]);
}
RfAlgorithmInterface::Scalar GeneralizedRfAlgo::calc_tree_info_content(const SplitList &S) {
	Scalar sum = 0;
	for (size_t i = 0; i < S.size(); ++i) {
		sum += S[i].getHInfoContent();
	}
	return sum;
}
void GeneralizedRfAlgo::calc_pairwise_tree_dist(const std::vector<FastSplitList> &trees,
                                                RfMetricInterface::Results &res) {
	std::vector<GeneralizedRfAlgo::Scalar> tree_info(trees.size());
	for (size_t i = 0; i < trees.size(); ++i) {
		tree_info[i] = calc_tree_info_content(trees[i]);
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
GeneralizedRfAlgo::GeneralizedRfAlgo(size_t split_len) : pairwise_split_scores(0) {
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
		{
			// pre-calc
			unique_pll_splits.back().perform_popcount_precalc(PllSplit::split_len);
			const size_t a = unique_pll_splits.back().getPrecalcPopcnt();
			unique_pll_splits.back().setHInfoContent(h_info_content(a, taxa - a));
		}

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
		    << total_splits << " possible PllSplits. Duplicate ratio: " << duplicate_ratio
		    << " Number of unique splits: " << unique_pll_splits.size();
	}
	// unique_pll_splits will no longer reallocate -> write base-ptr to Static variable
	FastSplitList::setBasePtr(&unique_pll_splits[0]);
	return returnList;
}

SymmetricMatrix<GeneralizedRfAlgo::Scalar> GeneralizedRfAlgo::calcPairwiseSplitScores() {
	assert(!unique_pll_splits.empty());
	factorials.reserve(taxa + taxa + 4);

	size_t split_num = unique_pll_splits.size();
	SymmetricMatrix<Scalar> resMtx(split_num);
	for (size_t row = 0; row < split_num; ++row) {
		auto &rSplit = unique_pll_splits[row];
		// let the split know, which index it is. -> should happen once per PllSplit
		rSplit.setIntersectionIdx(row);
		for (size_t col = 0; col < row; ++col) {
			auto &cSplit = unique_pll_splits[col];
			resMtx.set_at(row, col, calc_split_score(rSplit, cSplit));
		}
		resMtx.set_at(row, row, calc_split_score(rSplit));
	}
	return resMtx;
}
