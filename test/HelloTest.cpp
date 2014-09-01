#include "gmock/gmock.h"

using ::testing::Eq;

TEST(Hello, Test) {
  ASSERT_THAT(1, Eq(1));
}