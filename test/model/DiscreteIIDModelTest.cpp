#include "gmock/gmock.h"

#include "DiscreteIIDModel.hpp"

using ::testing::Eq;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;

class ADiscreteIIDModel : public testing::Test {
protected:
  DiscreteIIDModelPtr model = DiscreteIIDModelPtr(new DiscreteIIDModel({1,2,3}));
};

TEST_F(ADiscreteIIDModel, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(1, Eq(1));
}
