#include "gmock/gmock.h"

#include "ContextTree.hpp"
#include "DiscreteIIDModel.hpp"

#include <math.h>

using ::testing::Eq;
using tops::model::ContextTree;
using tops::model::ContextTreePtr;
using tops::model::ContextTreeNode;
using tops::model::ContextTreeNodePtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;

class AContextTree : public testing::Test {
protected:
  DiscreteIIDModelPtr d1 = DiscreteIIDModelPtr(new DiscreteIIDModel({0.5,0.3,0.2}));
  ContextTreeNodePtr n1;
  DiscreteIIDModelPtr d2 = DiscreteIIDModelPtr(new DiscreteIIDModel({0.4,0.4,0.3}));
  ContextTreeNodePtr n2;
  DiscreteIIDModelPtr d3 = DiscreteIIDModelPtr(new DiscreteIIDModel({0.3,0.3,0.4}));
  ContextTreeNodePtr n3;
  DiscreteIIDModelPtr d4 = DiscreteIIDModelPtr(new DiscreteIIDModel({0.7,0.2,0.1}));
  ContextTreeNodePtr n4;

  ContextTreePtr tree = ContextTreePtr(new ContextTree(3));

  virtual void SetUp() {
    n1 = tree->createContext(d1);
    n2 = tree->createContext(d2);
    n3 = tree->createContext(d3);
    n4 = tree->createContext(d4);

    n1->child(n2, 0);
    n1->child(n3, 1);
    n3->child(n4, 0);
  }
};

TEST_F(AContextTree, ShouldHaveARootNode) {
  ASSERT_THAT(tree->root()->_self, Eq(n1->_self));
}

TEST_F(AContextTree, ShouldHaveARootNodeThatHasChildren) {
  ASSERT_THAT(tree->root()->child(0)->_self, Eq(n2->_self));
  ASSERT_THAT(tree->root()->child(1)->_self, Eq(n3->_self));
  ASSERT_THAT(tree->root()->child(1)->child(0)->_self, Eq(n4->_self));
}
