#ifndef RF_METRICS_TWOSPLITHASHMAPFUNCTOR_H
#define RF_METRICS_TWOSPLITHASHMAPFUNCTOR_H
#include "TwoSplitHashmapKey.h"
struct TwoSplitHashmapFunctor {
    std::size_t operator()(const TwoSplitHashmapKey &key) const;
};

#endif // RF_METRICS_TWOSPLITHASHMAPFUNCTOR_H
