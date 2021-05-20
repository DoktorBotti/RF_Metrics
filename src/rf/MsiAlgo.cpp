//
// Created by Tobia on 16.05.2021.
//

#include "MsiAlgo.h"

double
MsiAlgo::calc_split_score(const PllSplit &S1, const PllSplit &S2, size_t taxa, size_t split_len) {
	// TODO: could save one interim result with boolean transformation -> test performance
	// TODO: are there ways to predict which calculation returns the greater value?
	auto split_buffer = GeneralizedRfAlgo::compute_split_comparison(S1, S2, split_len);

	const auto bits_too_many = GeneralizedRfAlgo::bits_too_many(taxa);
	const auto a1_a2 = PllSplit(&split_buffer[2 * split_len]).popcount(split_len);
	const auto b1_b2 = PllSplit(&split_buffer[3 * split_len]).popcount(split_len) - bits_too_many;
	const auto a1_b2 = PllSplit(&split_buffer[4 * split_len]).popcount(split_len);
	const auto a2_b1 = PllSplit(&split_buffer[5 * split_len]).popcount(split_len);

	return std::max(h_info_content(a1_a2, b1_b2), h_info_content(a1_b2, a2_b1));
}
