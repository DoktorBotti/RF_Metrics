
#ifndef INFORF_RFALGORITHMINTERFACE_H
#define INFORF_RFALGORITHMINTERFACE_H
#include "PllTree.hpp"
#include "RectMatrix.hpp"
#include "RfMetricInterface.h"
#include <vector>
class RfAlgorithmInterface {
  public:
	typedef double Scalar;
	virtual RfMetricInterface::Results calculate(std::vector<PllTree> &trees) = 0;
	// avoid v-table issues by defining trivial destructor in its own compile unit
	virtual ~RfAlgorithmInterface();

    struct SplitScores {
        explicit SplitScores(size_t num_trees);
        SplitScores(const SplitScores& ) = default;
        SplitScores(SplitScores&& ) = default;
		explicit SplitScores(RectMatrix<Scalar> &&matrix);
		~SplitScores() = default;

        Scalar max_score = 0;
        RectMatrix<Scalar> scores;
    };
};


#endif // INFORF_RFALGORITHMINTERFACE_H
