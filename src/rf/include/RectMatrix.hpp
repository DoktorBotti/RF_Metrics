#ifndef INFORF_RECTMATRIX_H
#define INFORF_RECTMATRIX_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <vector>

template <typename T>
class RectMatrix {
  public:
	explicit RectMatrix(size_t num_elems);
	/* Sets the value at row, column. */
	void set(size_t row, size_t column, T val);
	/* Retrieves the value at row, column. */
	T at(size_t row, size_t column) const;
	[[nodiscard]] size_t size() const;
	std::vector<std::vector<T>> to_vector() const;

	[[nodiscard]] std::pair<T, T> get_min_max() const;
	[[nodiscard]] std::string print() const;

  private:
	std::vector<T> matrix;
	size_t dim;
};

// ------------- implementation ------------------

template <typename T>
RectMatrix<T>::RectMatrix(size_t num_elems) : matrix(num_elems * num_elems, 0) {
	dim = num_elems;
}

template <typename T>
void RectMatrix<T>::set(size_t row, size_t column, T val) {
	assert(row < dim);
	assert(column < dim);

	matrix[row * dim + column] = val;
}

template <typename T>
T RectMatrix<T>::at(size_t row, size_t column) const {
	assert(row < dim);
	assert(column < dim);
	return matrix[row * dim + column];
}

template <typename T>
size_t RectMatrix<T>::size() const {
	return dim;
}

template <typename T>
std::vector<std::vector<T>> RectMatrix<T>::to_vector() const {
	std::vector<std::vector<T>> res;
	for (size_t row = 0; row < dim; ++row) {
		res.emplace_back();
		auto start = matrix.begin() + row * dim;
		auto end = matrix.begin() + static_cast<long>((row + 1) * dim);
		res[row].assign(start, end);
	}
	return res;
}
template <typename T>
std::pair<T, T> RectMatrix<T>::get_min_max() const {
	auto min_max = std::minmax_element(matrix.begin(), matrix.end());
	return std::make_pair(*min_max.first, *min_max.second);
}
template <typename T>
std::string RectMatrix<T>::print() const {
	std::stringstream ss;
	auto min_max = get_min_max();
	long digits_pre_comma = std::log(min_max.second);
	// expect one digit before comma. For all additional 4 digits, insert tab
	std::string spacer = "\t\t";
	for (size_t i = 0; i < static_cast<size_t>(std::max(digits_pre_comma / 4 - 1, 0l)); ++i) {
		spacer += "\t";
	}
	for (size_t row = 0; row < size(); ++row) {
		for (size_t col = 0; col < size(); ++col) {
			const auto &val = at(row, col);
			ss << std::setprecision(5) << val << (val == 0 ? spacer : " ");
		}
		ss << "\n";
	}
	return ss.str();
}

#endif // INFORF_RECTMATRIX_H
