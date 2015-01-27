/***********************************************************************/
/*  Copyright 2015 ToPS                                                */
/*                                                                     */
/*  This program is free software; you can redistribute it and/or      */
/*  modify it under the terms of the GNU  General Public License as    */
/*  published by the Free Software Foundation; either version 3 of     */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This program is distributed in the hope that it will be useful,    */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of     */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      */
/*  GNU General Public License for more details.                       */
/*                                                                     */
/*  You should have received a copy of the GNU General Public License  */
/*  along with this program; if not, write to the Free Software        */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,         */
/*  MA 02110-1301, USA.                                                */
/***********************************************************************/

#include <math.h>
#include <vector>

#include "gmock/gmock.h"

#include "InhomogeneousMarkovChain.hpp"
#include "VariableLengthMarkovChain.hpp"
#include "Sequence.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;

using tops::model::Sequence;
using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::InhomogeneousMarkovChain;
using tops::model::InhomogeneousMarkovChainPtr;

VariableLengthMarkovChainPtr createMC() {
  auto alphabet = tops::AlphabetPtr(new tops::Alphabet());
  alphabet->createSymbol("0");
  alphabet->createSymbol("1");
  auto context_tree = tops::ContextTreePtr(new tops::ContextTree(alphabet));
  auto c0 = context_tree->createContext();
  auto p0 = {0.5, 0.5};
  auto iid0 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p0));
  c0->setDistribution(iid0);
  auto c1 = context_tree->createContext();
  auto p1 = {0.1, 0.9};
  auto iid1 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p1));
  c1->setDistribution(iid1);
  c0->setChild(c1, 0);

  return VariableLengthMarkovChain::make(context_tree);
}

VariableLengthMarkovChainPtr createVLMC() {
  auto alphabet = tops::AlphabetPtr(new tops::Alphabet());
  alphabet->createSymbol("0");
  alphabet->createSymbol("1");
  auto context_tree = tops::ContextTreePtr(new tops::ContextTree(alphabet));
  auto c0 = context_tree->createContext();
  auto p0 = {0.5, 0.5};
  auto iid0 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p0));
  c0->setDistribution(iid0);
  auto c1 = context_tree->createContext();
  auto p1 = {0.6, 0.4};
  auto iid1 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p1));
  c1->setDistribution(iid1);
  auto c2 = context_tree->createContext();
  auto p2 = {0.3, 0.7};
  auto iid2 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p2));
  c2->setDistribution(iid2);
  auto c3 = context_tree->createContext();
  auto p3 = {0.7, 0.3};
  auto iid3 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p3));
  c3->setDistribution(iid3);
  auto c4 = context_tree->createContext();
  auto p4 = {0.1, 0.9};
  auto iid4 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p4));
  c4->setDistribution(iid4);
  auto c5 = context_tree->createContext();
  auto p5 = {0.3, 0.7};
  auto iid5 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p5));
  c5->setDistribution(iid5);
  auto c6 = context_tree->createContext();
  auto p6 = {0.7, 0.3};
  auto iid6 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p6));
  c6->setDistribution(iid6);

  c0->setChild(c1, 0);
  c0->setChild(c2, 1);
  c2->setChild(c3, 0);
  c2->setChild(c4, 1);
  c3->setChild(c5, 0);
  c3->setChild(c6, 1);

  return VariableLengthMarkovChain::make(context_tree);
}

class AInhomogeneousMarkovChain : public testing::Test {
 protected:
  InhomogeneousMarkovChainPtr imc;

  virtual void SetUp() {
    imc = InhomogeneousMarkovChain::make({createVLMC(), createMC()}, true);
  }
};

TEST_F(AInhomogeneousMarkovChain, ShouldEvaluateASequence) {
  ASSERT_THAT(imc->evaluate({0}, 0, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(imc->evaluate({1}, 0, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(imc->evaluate({0, 1}, 0, 1), DoubleEq(log(0.45)));
  ASSERT_THAT(imc->evaluate({0, 0}, 0, 1), DoubleEq(log(0.05)));
  ASSERT_THAT(imc->evaluate({1, 0}, 0, 1), DoubleEq(log(0.25)));
  ASSERT_THAT(imc->evaluate({1, 1}, 0, 1), DoubleEq(log(0.25)));
  ASSERT_THAT(imc->evaluate({1, 0, 1}, 0, 2), DoubleEq(log(0.1)));
}
