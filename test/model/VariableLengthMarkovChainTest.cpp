#include "gmock/gmock.h"

#include "VariableLengthMarkovChain.hpp"

#include <math.h>

using ::testing::Eq;
using ::testing::DoubleEq;

using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::ContextTree;
using tops::model::ContextTreePtr;
using tops::model::ContextTreeNode;
using tops::model::ContextTreeNodePtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;

class AVLMC : public testing::Test {
protected:
  
  VariableLengthMarkovChainPtr model;

  DiscreteIIDModelPtr d1 = DiscreteIIDModelPtr(new DiscreteIIDModel({0.5, 0.5}));
  ContextTreeNodePtr n1;
  DiscreteIIDModelPtr d2 = DiscreteIIDModelPtr(new DiscreteIIDModel({0.6, 0.4}));
  ContextTreeNodePtr n2;
  DiscreteIIDModelPtr d3 = DiscreteIIDModelPtr(new DiscreteIIDModel({0.3, 0.7}));
  ContextTreeNodePtr n3;
  DiscreteIIDModelPtr d4 = DiscreteIIDModelPtr(new DiscreteIIDModel({0.7, 0.3}));
  ContextTreeNodePtr n4;
  DiscreteIIDModelPtr d5 = DiscreteIIDModelPtr(new DiscreteIIDModel({0.1, 0.9}));
  ContextTreeNodePtr n5;
  DiscreteIIDModelPtr d6 = DiscreteIIDModelPtr(new DiscreteIIDModel({0.3, 0.7}));
  ContextTreeNodePtr n6;
  DiscreteIIDModelPtr d7 = DiscreteIIDModelPtr(new DiscreteIIDModel({0.7, 0.3}));
  ContextTreeNodePtr n7;

  ContextTreePtr tree = ContextTreePtr(new ContextTree(2));

  virtual void SetUp() {
    n1 = tree->createContext(d1);
    n2 = tree->createContext(d2);
    n1->child(n2, 0);
    n3 = tree->createContext(d3);
    n1->child(n3, 1);
    n4 = tree->createContext(d4);
    n3->child(n4, 0);
    n5 = tree->createContext(d5);
    n3->child(n5, 1);
    n6 = tree->createContext(d6);
    n4->child(n6, 0);
    n7 = tree->createContext(d7);
    n4->child(n7, 1);

    model = VariableLengthMarkovChainPtr(new VariableLengthMarkovChain(tree, 2));
  }
};

TEST_F(AVLMC, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(model->alphabetSize(), Eq(2));
}

TEST_F(AVLMC, ShouldEvaluateASequence) {
  ASSERT_THAT(model->evaluate({0}, 0, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(model->evaluate({1}, 0, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(model->evaluate({0, 1}, 0, 1), DoubleEq(log(0.20)));
  ASSERT_THAT(model->evaluate({0, 0}, 0, 1), DoubleEq(log(0.30)));
  ASSERT_THAT(model->evaluate({1, 0}, 0, 1), DoubleEq(log(0.15)));
  ASSERT_THAT(model->evaluate({1, 1}, 0, 1), DoubleEq(log(0.35)));
  ASSERT_THAT(model->evaluate({1, 0, 1}, 0, 2), DoubleEq(log(0.06)));
}

TEST_F(AVLMC, ShouldHaveEvaluateASequencePosition) {
  ASSERT_THAT(model->evaluatePosition({0}, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(model->evaluatePosition({1}, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(model->evaluatePosition({0, 1}, 1), DoubleEq(log(0.40)));
  ASSERT_THAT(model->evaluatePosition({0, 0}, 1), DoubleEq(log(0.60)));
  ASSERT_THAT(model->evaluatePosition({1, 0}, 1), DoubleEq(log(0.30)));
  ASSERT_THAT(model->evaluatePosition({1, 1}, 1), DoubleEq(log(0.70)));
  ASSERT_THAT(model->evaluatePosition({1, 0, 1}, 2), DoubleEq(log(0.40)));
}
