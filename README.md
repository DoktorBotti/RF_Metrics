# Introduction

Skeleton for the 2021 KIT bioinformatics practical. This repository contains the bare bones start for computing a
distance function on trees using splits. To get started, first clone the repository:

```
git clone --recursive git@codeberg.org:Exelixis-Lab/practical-2021.git
```

After that, to build the code run `make && cd build && make`, which will generate the build directory, and then start
building the code. The static library that is built is called `libinforf`, and is placed in `bin` after building.

I suggest that you take a look at the code itself, as you will be expected to modify it directly. The 2 header files,
`src/PllSplits.hpp` and `src/PllTree.hpp` 

# Splits Data Structure

The individual splits are represented as bitvectors, which are stored as `pll_split_base_t` (which is an alias of
`unsigned int`). The order of the bits in the bit vector is ensured by matching internal IDs. I have written a simple
wrapper class around the splits, which can be used to implement the basic operations required. A non exhaustive list of
suggested operations is:

- popcount
- bitextract
- union (bitwise or)
- intersect (bitwise and)
- symmetric difference (bitwise xor)

# Actually computing the splits

To actually compute the splits, something like the following should be done

```
std::vector<PllTree> tree_list;
for(auto t: tree_strings){
    tree_list.emplace_back(t);
}

std::vector<PllSplits> splits_list;

for(auto &t: tree_list){
    t.alignNodeIndices(*tree_list.begin());
    splits_list.emplace_back(t);
}
```

The important function to apply is the `alignNodeIndices`. This makes sure that the bitvectors are ordered the proper
way, so that the results make sense.

This example can be found more fleshed out in `test/src/main.cpp`.

# Possibly useful functions from libpll

```
void pllmod_utree_split_show(pll_split_t split, unsigned int tip_count)
```

Prints the split.
