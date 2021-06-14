#include "../../src/rf/helpers/Util.h"
#include "RndTreeGenerator.h"
#include "catch2/catch.hpp"
#include <MciAlgo.h>
#include <MsiAlgo.h>
#include <SpiAlgo.h>
#include <iostream>

TEST_CASE("Print random trees", "[Sym]") {
	RndTreeGenerator::cli_options_t creation_opts;
	creation_opts.trees = 189;
	creation_opts.taxa = 7;
	auto res_str = RndTreeGenerator::create_random_trees(creation_opts);

	CHECK(Util::split(res_str, '\n').size() == 189);
	CHECK(Util::split(Util::split(res_str, '\n').front(), ',').size() == 7);
}
static inline void checkSymmetry(GeneralizedRfAlgo &algo,
                          const PllSplit &split_a,
                          const PllSplit &split_b,
                          const size_t num_taxa,
                          const size_t split_len);
TEST_CASE("Find un-symmetric compatible() method", "[Sym]")
{
	// config
	const size_t num_taxa = GENERATE(14,18,21,80);

	// create random trees, many
	RndTreeGenerator::cli_options_t creation_opts;
	creation_opts.trees = 100;
	creation_opts.taxa = num_taxa;
	auto res_str = RndTreeGenerator::create_random_trees(creation_opts);
	auto trees = Util::create_all_trees_from_string(res_str);
	// extract splits. Each tree now identifies by its index in all_splits
	std::vector<PllSplitList> all_splits;
	all_splits.reserve(trees.size());
	for (auto &t : trees) {
		t.alignNodeIndices(*trees.begin());
		all_splits.emplace_back(t);
	}
	const size_t split_len = all_splits.front().computeSplitLen();
	SpiAlgo algo_spi;
	MciAlgo algo_mci;
	MsiAlgo algo_msi;

	// iterate through all tree combinations
	for (size_t idx_a = 0; idx_a < all_splits.size(); ++idx_a) {
		for (size_t idx_b = 0; idx_b <= idx_a; ++idx_b) {
			const auto &tree_a = all_splits[idx_a];
			const auto &tree_b = all_splits[idx_b];
			// iterate through all split combinations IN SYMMETRIC WAY. This is the way
			for (size_t spl_a = 0; spl_a < all_splits.front().size(); ++spl_a) {
				for (size_t spl_b = 0; spl_b <= spl_a; ++spl_b) {
					// check score symmetric
					auto split_a = tree_a[spl_a];
					auto split_b = tree_b[spl_b];
					checkSymmetry(algo_mci, split_a, split_b, num_taxa, split_len);
					checkSymmetry(algo_spi, split_a, split_b, num_taxa, split_len);
					checkSymmetry(algo_msi, split_a, split_b, num_taxa, split_len);
				}
			}
		}
	}
}
static void checkSymmetry(GeneralizedRfAlgo &algo,
                          const PllSplit &split_a,
                          const PllSplit &split_b,
                          const size_t num_taxa,
                          const size_t split_len) {
	double score_a = algo.calc_split_score(split_a, split_b, num_taxa, split_len);
	double score_b = algo.calc_split_score(split_b, split_a, num_taxa, split_len);
	if(std::abs(score_a - score_b)> 1e-8) {
		pllmod_utree_split_show(split_a(), num_taxa);
		putchar('\n');
		pllmod_utree_split_show(split_b(), num_taxa);
		putchar('\n');
		putchar('\n');
	}


	CHECK(std::abs(score_a - score_b)<= 1e-8);
}
