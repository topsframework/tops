#include "gmock/gmock.h"

#include "ContextTreeNode.hpp"
#include "DiscreteIIDModel.hpp"

#include <math.h>

using ::testing::Eq;
using tops::model::ContextTreeNode;
using tops::model::ContextTreeNodePtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;

class AContextTreeNode : public testing::Test {
protected:
  DiscreteIIDModelPtr distribution = DiscreteIIDModelPtr(new DiscreteIIDModel({0.5,0.3,0.2}));
  ContextTreeNodePtr node = ContextTreeNodePtr(new ContextTreeNode(distribution));
};

TEST_F(AContextTreeNode, ShouldHaveADistribution) {
  ASSERT_THAT(node->distribution(), Eq(distribution));
}

TEST_F(AContextTreeNode, ShouldHaveChildren) {
  auto c0 = node->child(ContextTreeNodePtr(new ContextTreeNode(distribution)), 0);
  auto c1 = node->child(ContextTreeNodePtr(new ContextTreeNode(distribution)), 1);
  auto c2 = node->child(ContextTreeNodePtr(new ContextTreeNode(distribution)), 2);
  ASSERT_THAT(node->child(0), Eq(c0));
  ASSERT_THAT(node->child(1), Eq(c1));
  ASSERT_THAT(node->child(2), Eq(c2));
}