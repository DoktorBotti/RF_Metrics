#include "../../src/rf/helpers/Util.h"
#include "PllTree.hpp"
#include <HashmapUtil/Cacher.h>
#include <catch2/catch.hpp>
#include <string.h>

TEST_CASE("Key equality", "[HashmapUtil]") {
	std::string tree_str = "((((F,C),B),A),(E,(D,G)));";
	auto tree = Util::create_all_trees_from_string(tree_str)[0];
	auto split_list = PllSplitList(tree);
	PllSplit a(split_list[0]);
	PllSplit b(split_list[1]);
	PllSplit c(split_list[2]);
	PllSplit d(split_list[3]);
	// completely equal or unequal
	{
		PllSplit::split_len = 1;
		TwoSplitHashmapKey k;
		auto key_a = TwoSplitHashmapKey(&a, &b);
		auto key_b = TwoSplitHashmapKey(&c, &d);
		CHECK(key_a != key_b);
		CHECK(key_a == key_a);

		auto key_perm_a = TwoSplitHashmapKey(&b, &a);
		CHECK(key_b != key_perm_a);
		CHECK(key_a == key_perm_a);
	}
    // one differs
    {
        auto key_a = TwoSplitHashmapKey(&a, &b);
        auto key_b = TwoSplitHashmapKey(&a, &d);
        CHECK(key_a != key_b);
        CHECK(key_a == key_a);
        auto key_perm_a = new TwoSplitHashmapKey(&b, &a);
        CHECK(key_b != *key_perm_a);
		delete key_perm_a;
    }
}

TEST_CASE("Cache some values", "[HashmapUtil]") { // Two statements according to KWStyle?!
	Cacher cache;
    std::string tree_str = "((((F,C),B),A),(E,D));";
    auto tree = Util::create_all_trees_from_string(tree_str)[0];
    auto split_list = PllSplitList(tree);
	PllSplit::split_len = 1;
    PllSplit a(split_list[0]);
    PllSplit b(split_list[1]);
    //PllSplit c(split_list[2]);
    //PllSplit d(split_list[3]);
	auto key = TwoSplitHashmapKey(&a,&b);
	double outVal = 0.;
	cache.lookupTrees(key, outVal);
}