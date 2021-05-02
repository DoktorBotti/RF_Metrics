//
// Created by Tobias Bodmer on 02.05.2021.
//

#ifndef INFORF_SYMMETRICMATRIX_H
#define INFORF_SYMMETRICMATRIX_H

#include <cstdio>
class SymmetricMatrix {
  public:
    explicit SymmetricMatrix(size_t num_elems);
    void setAt(size_t row, size_t column);
};

#endif // INFORF_SYMMETRICMATRIX_H
