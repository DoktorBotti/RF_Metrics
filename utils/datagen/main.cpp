#include <fstream>
extern "C" {
#include "libpll/pll_tree.h"
}
#include <cstddef>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <random>
#include <stdexcept>

template <typename T> class cli_option_t {
public:
  cli_option_t() : _value{}, _set{false} {}
  cli_option_t(const T &v) : _value{v}, _set{false} {}

  T get() {
    if (!_set) { throw std::runtime_error{"Option was not set"}; }
    return _value;
  }

  bool set() { return _set; }

  cli_option_t<T> &operator=(const T &v) {
    _value = v;
    _set   = true;
    return *this;
  }

  operator T() const {
    if (!_set) { throw std::runtime_error{"Value was not set"}; }
    return _value;
  }

private:
  T    _value;
  bool _set;
};

struct cli_options_t {
  cli_option_t<size_t>       trees;
  cli_option_t<size_t>       taxa;
  cli_option_t<unsigned int> seed;
};

constexpr size_t ipow(size_t n, size_t e) {
  size_t acc = 1;
  for (size_t i = 0; i < e; ++i) { acc *= n; }
  return acc;
}

template <size_t B = 26> size_t compute_base_length(size_t n) {
  if (n == 0) return 0;
  size_t high = 1;
  size_t len  = 0;

  while (n >= high) {
    high *= B;
    if (len == 0) high += 1;
    if (len > 0) { n -= ipow(B, len - 1); }

    len += 1;
  }

  return len;
}

char *base26_label(size_t n) {
  if (n == 0) { return nullptr; }
  static constexpr size_t B = 26;

  size_t len = compute_base_length<B>(n);
  size_t tmp = 1;
  for (size_t i = 0; i < len; ++i) {
    n -= tmp / B;
    tmp *= B;
  }

  char *label = (char *)calloc(len + 1, sizeof(char));

  for (int i = len - 1; i >= 0; --i) {
    char d = 'a' + (n % B);
    if (i == 0) { d = 'a' + (n % (B + 1)) - 1; }
    label[i] = d;

    n /= B;
  }

  return label;
}

char **generate_labels(size_t taxa) {
  char **labels = (char **)calloc(taxa, sizeof(char *));
  for (size_t i = 0; i < taxa; ++i) { labels[i] = base26_label(i + 1); }
  return labels;
}

void print_help() {
  std::cout
      << "Usage: --count [NUMBER] --groups [NUMBER]" << std::endl
      << "Will produce <GROUP> sets of trees with <COUNT> elements. Each set"
      << std::endl
      << "is seeded with a tree, which is then randomly permuted to produce "
         "the set"
      << std::endl;
}

cli_options_t parse_cli_options(int argc, char **argv) {
  cli_options_t                  opts;
  static constexpr struct option long_options[] = {
      {"help", no_argument, nullptr, 0},        /* 0 */
      {"trees", required_argument, nullptr, 0}, /* 1 */
      {"taxa", required_argument, nullptr, 0},  /* 2 */
      {"seed", required_argument, nullptr, 0},  /* 3 */
      {0, 0, 0, 0}};
  int  option_index = 0;
  char c;
  while ((c = getopt_long(argc, argv, "", long_options, &option_index)) == 0) {
    switch (option_index) {
    case 0:
      print_help();
      std::exit(0);
    case 1:
      opts.trees = atoll(optarg);
      break;
    case 2:
      opts.taxa = atoll(optarg);
      break;
    case 3:
      opts.seed = atoll(optarg);
      break;
    case ':':
    case '?':
      print_help();
      std::exit(1);
    default:
      throw std::invalid_argument{"An arguement was not recognized"};
    }
  }
  return opts;
}

int main(int argc, char **argv) {
  auto opts = parse_cli_options(argc, argv);

  if (!opts.seed.set()) { opts.seed = std::random_device()(); }

  auto labels = generate_labels(opts.taxa);

  std::default_random_engine gen(opts.seed);

  auto newick_cb = [](const pll_unode_t *n) {
    size_t label_len;
    if (n->label == nullptr) {
      label_len = 1;
    } else {
      label_len = strlen(n->label);
    }

    char *label = (char *)calloc(label_len, sizeof(char));

    if (n->label != nullptr) { memcpy(label, n->label, label_len); }
    return label;
  };

  for (size_t i = 0; i < opts.trees; i++) {
    auto random_tree = pllmod_utree_create_random(opts.taxa, labels, gen());

    auto newick_string = pll_utree_export_newick(random_tree->vroot, newick_cb);

    std::cout << newick_string << std::endl;

    free(newick_string);
  }

  for (size_t i = 0; i < opts.taxa; ++i) { free(labels[i]); }
  free(labels);

  return 0;
}
