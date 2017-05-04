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

#include "exception/NotYetImplemented.hpp"

#include "helper/DiscreteIIDModel.hpp"

// Tested header
#include "model/MaximalDependenceDecomposition.hpp"
#include "helper/MaximalDependenceDecomposition.hpp"

// Macros
#define DOUBLE(X) static_cast<double>(X)

/*----------------------------------------------------------------------------*/
/*                             USING DECLARATIONS                             */
/*----------------------------------------------------------------------------*/

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::ContainerEq;

using tops::model::Sequence;
using tops::model::INVALID_SYMBOL;
using tops::model::MaximalDependenceDecomposition;
using tops::model::MaximalDependenceDecompositionPtr;

using tops::exception::NotYetImplemented;

using tops::helper::createSampleMDD;
using tops::helper::createDNAIIDModel;
using tops::helper::createConsensusSequence;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class AMDD : public testing::Test {
 protected:
  MaximalDependenceDecompositionPtr mdd = createSampleMDD();
};

/*----------------------------------------------------------------------------*/
/*                                SIMPLE TESTS                                */
/*----------------------------------------------------------------------------*/

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
    4, createConsensusSequence(), createDNAIIDModel(), 2);

  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({1, 0, 2, 2, 3, 2, 0, 0, 3})
                        ->evaluateSequence(0, 9)),
              DoubleNear(0.0, 1e-4));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({1, 1, 2, 2, 3, 2, 0, 0, 3})
                        ->evaluateSequence(0, 9)),
              DoubleNear(0.0, 1e-4));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({1, 1, 3, 2, 3, 2, 0, 0, 0})
                        ->evaluateSequence(0, 9)),
              DoubleNear(0.0, 1e-4));
}

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(AMDD, ShouldEvaluateASymbol) {
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({0})->evaluateSymbol(0)),
              DoubleEq(0));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({1})->evaluateSymbol(0)),
              DoubleEq(0));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({0, 1})->evaluateSymbol(1)),
              DoubleEq(0));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({0, 0})->evaluateSymbol(1)),
              DoubleEq(0));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({1, 0})->evaluateSymbol(1)),
              DoubleEq(0));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({1, 1})->evaluateSymbol(1)),
              DoubleEq(0));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({1, 0, 1})->evaluateSymbol(2)),
              DoubleEq(0));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({1, 0, 1, 0})->evaluateSymbol(3)),
              DoubleEq(0));
}

/*----------------------------------------------------------------------------*/

TEST_F(AMDD, ShouldEvaluateASequence) {
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({0})->evaluateSequence(0, 1)),
              DoubleEq(0));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({1, 0, 2, 2, 3, 2, 0, 0, 3})
                        ->evaluateSequence(0, 9)),
              DoubleNear(7.67981648e-7, 1e-4));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({1, 2, 2, 2, 3, 2, 0, 2, 3})
                        ->evaluateSequence(0, 9)),
              DoubleNear(0.000012, 1e-4));
  ASSERT_THAT(DOUBLE(mdd->standardEvaluator({2, 2, 2, 2, 2, 2, 2, 2, 2})
                        ->evaluateSequence(0, 9)),
              DoubleNear(0.000262, 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST_F(AMDD, ShouldEvaluateASequenceWithPrefixSumArray) {
  std::vector<Sequence> sequences {
    Sequence{0},
    Sequence{1, 0, 2, 2, 3, 2, 0, 0, 3},
    Sequence{1, 2, 2, 2, 3, 2, 0, 2, 3},
    Sequence{2, 2, 2, 2, 2, 2, 2, 2, 2},
  };

  for (auto& sequence : sequences) {
    unsigned int size = sequence.size();
    ASSERT_THAT(DOUBLE(mdd->standardEvaluator(sequence, true)
                          ->evaluateSequence(0, size)),
                DoubleEq(DOUBLE(mdd->standardEvaluator(sequence)
                                   ->evaluateSequence(0, size))));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AMDD, ShouldDrawSequenceWithDefaultSeed) {
  ASSERT_THROW(mdd->standardGenerator()->drawSequence(5), NotYetImplemented);
}

/*----------------------------------------------------------------------------*/
