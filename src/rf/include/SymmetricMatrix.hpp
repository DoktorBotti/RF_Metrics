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
	void setAt(size_t row, size_t column, T val);
	T at(size_t row, size_t column);
	size_t size();

  private:
	std::vector<std::vector<T>> matrix;
	size_t dim;
};

// ------------- implementation ------------------

template <typename T>
SymmetricMatrix<T>::SymmetricMatrix(size_t num_elems) {
	dim = num_elems;
	for (size_t i = 0; i < num_elems; ++i) {
		matrix.emplace_back(i);
	}
}

template <typename T>
void SymmetricMatrix<T>::setAt(size_t row, size_t column, T val) {
	if (row >= column) {
		matrix[row][column] = val;
	} else {
		matrix[column][row] = val;
	}
}

template <typename T>
T SymmetricMatrix<T>::at(size_t row, size_t column) {
	return row >= column ? matrix[row][column] : matrix[column][row];
}

template <typename T>
size_t SymmetricMatrix<T>::size() {
	return dim;
}

#endif // INFORF_SYMMETRICMATRIX_H
