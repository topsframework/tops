#include "gmock/gmock.h"

#include "DiscreteIIDModel.hpp"

#include <math.h>

using ::testing::Eq;
using ::testing::DoubleEq;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::Sequence;

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

TEST_F(ADiscreteIIDModel, ShouldHaveEvaluateASequence) {
  std::vector<Sequence> test_data = {
    {0, 0, 1, 1, 2, 2},
    {0, 1, 2, 2, 2, 2},
    {1, 1, 1, 0, 0, 0}
  };
  for (auto data : test_data) {
    double result = 0.0;
    for (auto symbol : data) {
      result += model->logProbabilityOf(symbol);
    }
    ASSERT_THAT(model->evaluate(data, 0, 5), DoubleEq(result));
  }
}
