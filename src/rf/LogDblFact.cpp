#include "LogDblFact.h"
#include <cassert>
#include <cmath>

size_t LogDblFact::get_capacity() const {
	return num_reserved;
}
LogDblFact::Scalar LogDblFact::lg_fast(size_t x) const {
	assert(x < num_reserved);
	return cache_lg[x];
}
[[maybe_unused]] LogDblFact::Scalar LogDblFact::lg_dbl_fact_fast(long x) const {
	assert(static_cast<size_t>(std::abs(x)) < num_reserved);
	assert(x >= -1);
	return cache_fac[static_cast<size_t>(x + 1)];
}
LogDblFact::Scalar LogDblFact::lg_rooted_dbl_fact_fast(long x) const {
	assert(x >= 2);
	assert(static_cast<size_t>(x + x - 2) < num_reserved);
	return cache_fac[static_cast<size_t>(x + x - 2)];
}
LogDblFact::Scalar LogDblFact::lg_unrooted_dbl_fact_fast(long x) const {
	assert(x >= 1);
	assert(static_cast<size_t>(x + x - 4) < num_reserved);
	return cache_fac[static_cast<size_t>(x + x - 4)];
}
void LogDblFact::reserve(size_t num_els) {
	if (num_els <= num_reserved) {
		return;
	}
	cache_fac.reserve(num_els);
	cache_lg.reserve(num_els);
	calc_vals(num_els);
}
LogDblFact::Scalar LogDblFact::lg(size_t x) {
	if (num_reserved <= x) {
		reserve(static_cast<size_t>(x) * GROW_RATE);
	}
	return lg_fast(x);
}
LogDblFact::Scalar LogDblFact::lg_dbl_fact(long x) {
	assert(x > -2);
	if (num_reserved <= static_cast<size_t>(std::abs(x))) {
		reserve(static_cast<size_t>(x) * GROW_RATE);
	}
	return lg_dbl_fact_fast(x);
}
LogDblFact::Scalar LogDblFact::lg_rooted_dbl_fact(long x) {
	assert(x > -2);
	if (num_reserved <= static_cast<size_t>(std::abs(x + x - 2))) {
		reserve(static_cast<size_t>(x + x - 2) * GROW_RATE); // TODO: make (x - 1) << 1
	}
	return lg_rooted_dbl_fact_fast(x);
}
LogDblFact::Scalar LogDblFact::lg_unrooted_dbl_fact(long x) {
	assert(x > -2);
	if (num_reserved <= static_cast<size_t>(std::abs(x + x - 4))) {
        reserve(static_cast<size_t>(x + x - 4) * GROW_RATE); // TODO: make (x - 2) << 1
	}
	return lg_unrooted_dbl_fact_fast(x);
}

LogDblFact::LogDblFact() noexcept : cache_fac(PRECALC_NUM_ELEMS), cache_lg(PRECALC_NUM_ELEMS) {
	static_assert(PRECALC_NUM_ELEMS > 3);
	static_assert(GROW_RATE >= 1.4);
	cache_fac[0] = 0.; // lg2(-1!!)
	cache_fac[1] = 0.; // lg2(0!!)
	cache_fac[2] = 0.;
	cache_lg[0] = static_cast<double>(-INFINITY);
	cache_lg[1] = 0;
	cache_lg[2] = 1;
	// calc factorials and log from here
	calc_vals(PRECALC_NUM_ELEMS);
}
void LogDblFact::calc_vals(size_t new_num_els) {
	for (size_t x = 3; x < new_num_els; ++x) {
		cache_lg[x] = (std::log2(x));
		// calc lg((x-1)!!)
		cache_fac[x] = cache_fac[x - 2] + cache_lg[x - 1];
	}
	num_reserved = new_num_els;
}
