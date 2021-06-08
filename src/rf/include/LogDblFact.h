#ifndef INFORF_LOGDBLFACT_H
#define INFORF_LOGDBLFACT_H
#include <cstdio>
#include <vector>
// Class caches and calculates log2(x!!) and log2(x)
// it automatically manages its (instance dependent) cache size and offers different helpers for different offsets in the arguments
//TODO: should the cache be static & global? -> experiment in next phase
class LogDblFact {
  public:
	typedef double Scalar;
	LogDblFact() noexcept;
    [[nodiscard]] size_t get_capacity() const;
	void reserve(size_t num_els);
	// fast function do not check array boundaries. Caller must guarantee enough capacity

    // calculates log2(x), does not check for enough storage
    [[nodiscard]] Scalar lg_fast(size_t x) const;
    // calculates log2((x)!!), does not check for enough storage
    [[nodiscard]] Scalar lg_dbl_fact_fast(long x) const;
	// calculates log2((x+x-5)!!), does not check for enough storage
    [[nodiscard]] Scalar lg_rooted_dbl_fact_fast(long x) const;
    // calculates log2((x+x-3)!!), does not check for enough storage
    [[nodiscard]] Scalar lg_unrooted_dbl_fact_fast(long x) const;

	//non-fast: reserves if too large with grow rate
    [[nodiscard]] Scalar lg(size_t x);
    [[nodiscard]] Scalar lg_dbl_fact(long x);
    [[nodiscard]] Scalar lg_rooted_dbl_fact(long x);
    [[nodiscard]] Scalar lg_unrooted_dbl_fact(long x);
  private:
	const static size_t PRECALC_NUM_ELEMS = 100;
	constexpr static size_t GROW_RATE = 2;
	size_t num_reserved = PRECALC_NUM_ELEMS;
	// since -1!! exists, the array must start for x=-1
	std::vector<Scalar> cache_fac;
	std::vector<Scalar> cache_lg;
	void calc_vals(size_t new_num_els);
};

#endif // INFORF_LOGDBLFACT_H
