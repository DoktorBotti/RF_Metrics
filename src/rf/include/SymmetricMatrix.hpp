#ifndef INFORF_SYMMETRICMATRIX_H
#define INFORF_SYMMETRICMATRIX_H

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <sstream>
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
	const T &at(size_t row, size_t column) const;
	/* Retrieves the value at row, column. Checked, behaves like a symmetric matrix. */
	const T &checked_at(size_t row, size_t column) const;
	[[nodiscard]] size_t size() const;
	std::vector<std::vector<T>> to_vector() const;

	[[nodiscard]] std::pair<T, T> get_min_max() const;
	std::string print() const;

	SymmetricMatrix<T> operator-(const SymmetricMatrix<T> &other) const;

  private:
	std::vector<T> matrix;
	size_t dim;
};

// ------------- implementation ------------------

template <typename T>
SymmetricMatrix<T>::SymmetricMatrix(size_t num_elems) : matrix(num_elems * (num_elems + 1) / 2) {
	dim = num_elems;
}

template <typename T>
void SymmetricMatrix<T>::set_at(size_t row, size_t column, T val) {
	assert(row >= column);
	size_t row_idx = row * (row + 1) / 2;
	matrix[row_idx + column] = val;
}

template <typename T>
const T &SymmetricMatrix<T>::at(size_t row, size_t column) const {
	assert(row >= column);
	size_t row_idx = row * (row + 1) / 2;
	return matrix[row_idx + column];
}

template <typename T>
size_t SymmetricMatrix<T>::size() const {
	return dim;
}
template <typename T>
void SymmetricMatrix<T>::checked_set_at(size_t row, size_t column, T val) {
	if (row >= column) {
		size_t row_idx = row * (row + 1) / 2;
		matrix[row_idx + column] = val;
	} else {
		size_t column_idx = column * (column + 1) / 2;
		matrix[column_idx + row] = val;
	}
}
template <typename T>
const T &SymmetricMatrix<T>::checked_at(size_t row, size_t column) const {
	size_t idx = (row >= column) ? row * (row + 1) / 2 + column : column * (column + 1) / 2 + row;
	return matrix[idx];
}
template <typename T>
std::vector<std::vector<T>> SymmetricMatrix<T>::to_vector() const {
	std::vector<std::vector<T>> res;
	for (size_t row = 0; row < dim; ++row) {
		res.emplace_back();
		auto start = matrix.begin() + row * (row + 1) / 2;
		auto end = matrix.begin() + (row + 1) * (row + 2) / 2;
		res[row].assign(start, end);
	}
	return res;
}
template <typename T>
std::pair<T, T> SymmetricMatrix<T>::get_min_max() const {
	auto min_max = std::minmax_element(matrix.begin(), matrix.end());
	return std::make_pair(*min_max.first, *min_max.second);
}
template <typename T>
std::string SymmetricMatrix<T>::print() const {
	std::stringstream ss;
	for (size_t row = 0; row < size(); ++row) {
		for (size_t col = 0; col < size(); ++col) {
			ss << checked_at(row, col) << " ";
		}
		ss << "\n";
	}
	return ss.str();
}
template <typename T>
SymmetricMatrix<T> SymmetricMatrix<T>::operator-(const SymmetricMatrix<T> &other) const {
	assert(this->size() == other.size());
	SymmetricMatrix<T> out(this->size());
	for (size_t i = 0; i < this->matrix.size(); ++i) {
		out[i] = this->matrix[i] - other.matrix[i];
	}
	return out;
}

#endif // INFORF_SYMMETRICMATRIX_H
