#pragma once
#include "PllSplits.hpp"
#include "libpll/pll.h"
#include "libpll/pll_tree.h"
#include <exception>
#include <string>
#include <utility>

/* Why even make a class that stores the tree? Why not just have the splits
 * constructor take a newick string?
 *
 * Because you might want to make a tree -> splits map, or a splits -> treelist
 * map. At least the first example is used in "real" implementations of distance
 * functions on many trees.
 */
class PllTree {
public:
  PllTree(const std::string &newick_string);

  /* Rule of 5 constructors/destructors */
  ~PllTree();
  PllTree(const PllTree &other);
  PllTree(PllTree &&other) : _tree{std::exchange(other._tree, nullptr)} {}
  PllTree &operator=(const PllTree &other) { return *this = PllTree(other); }
  PllTree &operator=(PllTree &&other) {
    std::swap(_tree, other._tree);
    return *this;
  }

  /* Getters */

  const pll_utree_t *tree() const { return _tree; }

  /*Actually important functions */

  PllSplits makeSplits() const;

private:
  pll_utree_t *_tree;
};
