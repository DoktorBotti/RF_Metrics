#include <fstream>
#include "RndTreeGenerator.h"
extern "C" {
#include "libpll/pll_tree.h"
}
#include <cstddef>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>

void generate_random_trees(RndTreeGenerator::cli_options_t &opts);
constexpr size_t ipow(size_t n, size_t e) {
    size_t acc = 1;
    for (size_t i = 0; i < e; ++i) { acc *= n; }
    return acc;
}

template <size_t B = 26> size_t compute_base_length(size_t n) {
    if (n == 0) return 0;
    size_t high = 1;
    size_t len = 0;

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

std::string RndTreeGenerator::create_random_trees(RndTreeGenerator::cli_options_t &opts) {

	if (!opts.seed.set()) { opts.seed = std::random_device()();
	}

	auto labels = generate_labels(opts.taxa);
	std::default_random_engine gen(static_cast<unsigned>(opts.seed));

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
    std::stringstream ss;
	for (size_t i = 0; i < opts.trees; i++) {
        auto random_tree = pllmod_utree_create_random(opts.taxa, labels, gen());

        auto newick_string =
            pll_utree_export_newick(random_tree->vroot, newick_cb);

        ss << newick_string << std::endl;

        free(newick_string);
	}
    std::string result_str = ss.str();
	for (size_t i = 0; i < opts.taxa; ++i) { free(labels[i]);
	}
	free(labels);
	return result_str;
}
