//
// Created by Tobias Bodmer on 02.05.2021.
//

#ifndef INFORF_SYMMETRICMATRIX_H
#define INFORF_SYMMETRICMATRIX_H

#include <cstdio>
#include <vector>
#include <cassert>

template <typename T>
class SymmetricMatrix {
  public:
	explicit SymmetricMatrix(size_t num_elems);
	/* Sets the value at row, column. Unchecked, the caller must guarantee row >= column. */
	void set_at(size_t row, size_t column, T val);
	/* Sets the value at row, column. Checked, behaves like a symmetric matrix. */
	void checked_set_at(size_t row, size_t column, T val);
	/* Retrieves the value at row, column. Unchecked, the caller must guarantee row >= column. */
	T at(size_t row, size_t column) const;
	/* Retrieves the value at row, column. Checked, behaves like a symmetric matrix. */
	T checked_at(size_t row, size_t column) const;
	[[nodiscard]] size_t size() const;
    std::vector<std::vector<T>> to_vector() const;
  private:
	std::vector<T> matrix;
	size_t dim;
};

// ------------- implementation ------------------

template <typename T>
SymmetricMatrix<T>::SymmetricMatrix(size_t num_elems) : matrix(num_elems*(num_elems+1)/2,0){
	dim = num_elems;
}

template <typename T>
void SymmetricMatrix<T>::set_at(size_t row, size_t column, T val) {
	assert(row >= column);
	size_t row_idx = row*(row+1)/2;
	matrix[row_idx + column] = val;
}

template <typename T>
T SymmetricMatrix<T>::at(size_t row, size_t column) const {
	assert(row >= column);
    size_t row_idx = row*(row+1)/2;
    return matrix[row_idx + column];
}

template <typename T>
size_t SymmetricMatrix<T>::size() const {
	return dim;
}
template <typename T>
void SymmetricMatrix<T>::checked_set_at(size_t row, size_t column, T val) {
	if (row >= column) {
        size_t row_idx = row*(row+1)/2;
        matrix[row_idx + column] = val;
	} else {
        size_t column_idx = column*(column+1)/2;
        matrix[column_idx + row] = val;
	}
}
template <typename T>
T SymmetricMatrix<T>::checked_at(size_t row, size_t column) const {
	size_t idx = (row >= column)? row*(row+1)/2 + column : column * (column+1)/2 + row;
	return matrix[idx];
}
template <typename T>
std::vector<std::vector<T>> SymmetricMatrix<T>::to_vector() const {
	std::vector<std::vector<T>> res;
	for(size_t row = 0; row < dim; ++row){
		res.emplace_back();
		auto start = matrix.begin()+row*(row+1)/2;
		auto end = matrix.begin() + (row+1)*(row+2)/2;
		res[row].assign(start,end);
	}
	return res;
}

#endif // INFORF_SYMMETRICMATRIX_H
