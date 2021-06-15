
#include <RfAlgorithmInterface.h>

RfAlgorithmInterface::~RfAlgorithmInterface() {
}
RfAlgorithmInterface::SplitScores::SplitScores(size_t num_trees) : scores(num_trees) {
}
RfAlgorithmInterface::SplitScores::SplitScores(RectMatrix<Scalar> &&matrix) : scores(matrix) {
	auto min_max = scores.get_min_max();
	max_score = min_max.second;
}
