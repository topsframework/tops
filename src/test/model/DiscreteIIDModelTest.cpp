#include "gmock/gmock.h"

#include "DiscreteIIDModel.hpp"

#include <math.h>

using ::testing::Eq;
using ::testing::DoubleEq;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;

class ADiscreteIIDModel : public testing::Test {
protected:
  DiscreteIIDModelPtr model = DiscreteIIDModelPtr(new DiscreteIIDModel({0.5,0.3,0.2}));
};

TEST_F(ADiscreteIIDModel, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(model->alphabetSize(), Eq(3));
}

TEST_F(ADiscreteIIDModel, ShouldHaveEvaluateASingleSymbol) {
  ASSERT_THAT(model->logProbabilityOf(0), DoubleEq(log(0.5)));
  ASSERT_THAT(model->logProbabilityOf(1), DoubleEq(log(0.3)));
  ASSERT_THAT(model->logProbabilityOf(2), DoubleEq(log(0.2)));
}
