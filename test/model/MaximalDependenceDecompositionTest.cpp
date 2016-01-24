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

// Standard headers
#include <cmath>
#include <limits>
#include <vector>

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/Sequence.hpp"

#include "helper/DiscreteIIDModel.hpp"

// Tested header
#include "model/MaximalDependenceDecomposition.hpp"
#include "helper/MaximalDependenceDecomposition.hpp"

using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::Eq;
using ::testing::ContainerEq;

using tops::model::MaximalDependenceDecomposition;
using tops::model::MaximalDependenceDecompositionPtr;
using tops::model::Sequence;
using tops::model::INVALID_SYMBOL;

using tops::helper::createMDD;
using tops::helper::createDNAModel;
using tops::helper::createConsensusSequence;

class AMDD : public testing::Test {
 protected:
  MaximalDependenceDecompositionPtr mdd = createMDD();
};

TEST_F(AMDD, ShouldEvaluateASymbol) {
  ASSERT_THAT(mdd->standardEvaluator({0})->evaluateSymbol(0),
      DoubleEq(-std::numeric_limits<double>::infinity()));
  ASSERT_THAT(mdd->standardEvaluator({1})->evaluateSymbol(0),
      DoubleEq(-std::numeric_limits<double>::infinity()));
  ASSERT_THAT(mdd->standardEvaluator({0, 1})->evaluateSymbol(1),
      DoubleEq(-std::numeric_limits<double>::infinity()));
  ASSERT_THAT(mdd->standardEvaluator({0, 0})->evaluateSymbol(1),
      DoubleEq(-std::numeric_limits<double>::infinity()));
  ASSERT_THAT(mdd->standardEvaluator({1, 0})->evaluateSymbol(1),
      DoubleEq(-std::numeric_limits<double>::infinity()));
  ASSERT_THAT(mdd->standardEvaluator({1, 1})->evaluateSymbol(1),
      DoubleEq(-std::numeric_limits<double>::infinity()));
  ASSERT_THAT(mdd->standardEvaluator({1, 0, 1})->evaluateSymbol(2),
      DoubleEq(-std::numeric_limits<double>::infinity()));
  ASSERT_THAT(mdd->standardEvaluator({1, 0, 1, 0})->evaluateSymbol(3),
      DoubleEq(-std::numeric_limits<double>::infinity()));
}

TEST_F(AMDD, ShouldEvaluateASequence) {
  ASSERT_THAT(mdd->standardEvaluator({0})->evaluateSequence(0, 1),
              DoubleEq(-std::numeric_limits<double>::infinity()));
  ASSERT_THAT(mdd->standardEvaluator({1, 0, 2, 2, 3, 2, 0, 0, 3})
                 ->evaluateSequence(0, 9),
              DoubleNear(-14.0795, 1e-4));
  ASSERT_THAT(mdd->standardEvaluator({1, 2, 2, 2, 3, 2, 0, 2, 3})
                 ->evaluateSequence(0, 9),
              DoubleNear(-11.3069, 1e-4));
  ASSERT_THAT(mdd->standardEvaluator({2, 2, 2, 2, 2, 2, 2, 2, 2})
                 ->evaluateSequence(0, 9),
              DoubleNear(-8.24662, 1e-4));
}

TEST_F(AMDD, ShouldEvaluateASequenceWithPrefixSumArray) {
  std::vector<Sequence> sequences {
    Sequence{0},
    Sequence{1, 0, 2, 2, 3, 2, 0, 0, 3},
    Sequence{1, 2, 2, 2, 3, 2, 0, 2, 3},
    Sequence{2, 2, 2, 2, 2, 2, 2, 2, 2},
  };

  for (auto& sequence : sequences) {
    unsigned int size = sequence.size();
    ASSERT_THAT(mdd->standardEvaluator(sequence, true)
                   ->evaluateSequence(0, size),
                DoubleEq(mdd->standardEvaluator(sequence)
                            ->evaluateSequence(0, size)));
  }
}

TEST_F(AMDD, ShouldChooseSequenceWithDefaultSeed) {
  // TODO(igorbonadio): implement method
  ASSERT_THAT(mdd->standardGenerator()->drawSequence(5),
              ContainerEq(Sequence(5, INVALID_SYMBOL)));
}

TEST(MDD, ShouldBeTrained) {
  auto mdd_trainer = MaximalDependenceDecomposition::standardTrainer();

  mdd_trainer->add_training_set({{1, 0, 3, 1, 3, 2, 3, 0, 1},
                                 {0, 1, 2, 2, 3, 2, 3, 0, 2},
                                 {1, 0, 3, 2, 3, 1, 0, 0, 2},
                                 {0, 1, 2, 1, 3, 1, 0, 0, 3},
                                 {1, 0, 2, 2, 2, 1, 0, 1, 3},
                                 {0, 1, 3, 2, 3, 2, 0, 1, 3},
                                 {1, 0, 2, 1, 3, 2, 3, 1, 0}});

  auto mdd = mdd_trainer->train(
    MaximalDependenceDecomposition::standard_training_algorithm{},
    4, createConsensusSequence(), createDNAModel(), 2);

  ASSERT_THAT(mdd->standardEvaluator({1, 0, 2, 2, 3, 2, 0, 0, 3})
                 ->evaluateSequence(0, 9),
              DoubleNear(-6.45814, 1e-4));
  ASSERT_THAT(mdd->standardEvaluator({1, 1, 2, 2, 3, 2, 0, 0, 3})
                 ->evaluateSequence(0, 9),
              DoubleNear(-5.765, 1e-4));
  ASSERT_THAT(mdd->standardEvaluator({1, 1, 3, 2, 3, 2, 0, 0, 0})
                 ->evaluateSequence(0, 9),
              DoubleNear(-6.96784, 1e-4));
}
