#ifndef INFORF_SPLITLIST_H
#define INFORF_SPLITLIST_H

#include "PllSplit.hpp"
class SplitList{
  public:
    SplitList(const SplitList&) = default;
    SplitList(SplitList&&) = default;
    SplitList() = default;
    virtual ~SplitList();

    virtual const PllSplit& operator[](size_t index) const = 0;
    virtual PllSplit& operator[](size_t index)  = 0;
    [[nodiscard]] virtual size_t size() const = 0;

    /* Computes the number of pll_split_base_t's that are needed to store a
     * single split
     */
    [[nodiscard]] size_t computeSplitLen() const;


  protected:
    /* Computes the number of bits per split base */
    static constexpr size_t computeSplitBaseSize();

    [[nodiscard]] size_t computeSplitArraySize() const;

};


#endif // INFORF_SPLITLIST_H
