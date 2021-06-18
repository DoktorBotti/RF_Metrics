#ifndef INFORF_MATCHERHUNGARIAN_H
#define INFORF_MATCHERHUNGARIAN_H

#include "Matcher.h"

class MatcherHungarian : public Matcher
{
  public:
	Scalar solve(const RfAlgorithmInterface::SplitScores &scores) override;
};

#endif // INFORF_MATCHERHUNGARIAN_H
