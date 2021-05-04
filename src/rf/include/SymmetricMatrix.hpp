//
// Created by Tobias Bodmer on 02.05.2021.
//

#ifndef INFORF_SYMMETRICMATRIX_H
#define INFORF_SYMMETRICMATRIX_H

#include <cstdio>
#include <vector>
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
	size_t size();
    std::vector<std::vector<T>> to_vector() const;
  private:
	std::vector<std::vector<T>> matrix;
	size_t dim;
};

// ------------- implementation ------------------

template <typename T>
SymmetricMatrix<T>::SymmetricMatrix(size_t num_elems) {
	dim = num_elems;
	for (size_t i = 0; i < num_elems; ++i) {
		matrix.emplace_back(i + 1); // +1 to contain the diagonal elements
	}
}

template <typename T>
void SymmetricMatrix<T>::set_at(size_t row, size_t column, T val) {
	assert(row >= column);
	matrix[row][column] = val;
}

template <typename T>
T SymmetricMatrix<T>::at(size_t row, size_t column) const {
	assert(row >= column);
	return matrix[row][column];
}

template <typename T>
size_t SymmetricMatrix<T>::size() {
	return dim;
}
template <typename T>
void SymmetricMatrix<T>::checked_set_at(size_t row, size_t column, T val) {
	if (row >= column) {
		matrix[row][column] = val;
	} else {
		matrix[column][row] = val;
	}
}
template <typename T>
T SymmetricMatrix<T>::checked_at(size_t row, size_t column) const {
	return row >= column ? matrix[row][column] : matrix[column][row];
}
template <typename T>
std::vector<std::vector<T>> SymmetricMatrix<T>::to_vector() const {
	return matrix;
}

#endif // INFORF_SYMMETRICMATRIX_H
