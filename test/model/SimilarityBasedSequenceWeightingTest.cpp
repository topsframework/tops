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
#include <map>
#include <cmath>
#include <limits>
#include <vector>

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/Sequence.hpp"

// Tested header
#include "model/SimilarityBasedSequenceWeighting.hpp"

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
using tops::model::SimilarityBasedSequenceWeighting;
using tops::model::SimilarityBasedSequenceWeightingPtr;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class ASBSW : public testing::Test {
 protected:
  SimilarityBasedSequenceWeightingPtr sbsw;

  virtual void SetUp() {
    std::map<Sequence, unsigned int> counter;
    counter[{1, 1}] = 2;
    counter[{0, 0}] = 4;
    sbsw = SimilarityBasedSequenceWeighting::make(counter, 6, 0, 0,
                                                  Sequence{1, 0});
  }
};

/*----------------------------------------------------------------------------*/
/*                                SIMPLE TESTS                                */
/*----------------------------------------------------------------------------*/

TEST(SBSW, ShouldBeTrained) {
  auto sbsw_trainer = SimilarityBasedSequenceWeighting::standardTrainer();

  sbsw_trainer->add_training_set({{1, 1},
                                  {0, 1},
                                  {1, 1},
                                  {0, 0},
                                  {1, 1}});

  auto sbsw = sbsw_trainer->train(
    SimilarityBasedSequenceWeighting::standard_training_algorithm{},
    2, 0, 0, Sequence{});

  ASSERT_THAT(
      DOUBLE(sbsw->standardEvaluator({0})->evaluateSequence(0, 1)),
      DoubleEq(0));
  ASSERT_THAT(
      DOUBLE(sbsw->standardEvaluator({1})->evaluateSequence(0, 1)),
      DoubleEq(0));
  ASSERT_THAT(
      DOUBLE(sbsw->standardEvaluator({0, 1})->evaluateSequence(0, 2)),
      DoubleNear(0.200519, 1e-4));
  ASSERT_THAT(
      DOUBLE(sbsw->standardEvaluator({0, 0})->evaluateSequence(0, 2)),
      DoubleNear(0.19992 , 1e-4));
  ASSERT_THAT(
      DOUBLE(sbsw->standardEvaluator({1, 0})->evaluateSequence(0, 2)),
      DoubleNear(0.000799, 1e-4));
  ASSERT_THAT(
      DOUBLE(sbsw->standardEvaluator({1, 1})->evaluateSequence(0, 2)),
      DoubleNear(0.599361, 1e-4));
  ASSERT_THAT(
      DOUBLE(sbsw->standardEvaluator({1, 0, 1})->evaluateSequence(0, 3)),
      DoubleNear(0.000788, 1e-4));
  ASSERT_THAT(
      DOUBLE(sbsw->standardEvaluator({1, 0, 1, 0})->evaluateSequence(0, 4)),
      DoubleNear(0.000788, 1e-4));
  ASSERT_THAT(
      DOUBLE(sbsw->standardEvaluator({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
                 ->evaluateSequence(0, 13)),
             DoubleNear(0.000788, 1e-4));
}

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(ASBSW, ShouldEvaluateASequence) {
  ASSERT_THAT(
    DOUBLE(sbsw->standardEvaluator({0})->evaluateSequence(0, 1)),
           DoubleEq(0));
  ASSERT_THAT(
    DOUBLE(sbsw->standardEvaluator({1})->evaluateSequence(0, 1)),
           DoubleEq(0));
  ASSERT_THAT(
    DOUBLE(sbsw->standardEvaluator({0, 1})->evaluateSequence(0, 2)),
           DoubleNear(0.0009999, 1e-4));
  ASSERT_THAT(
    DOUBLE(sbsw->standardEvaluator({0, 0})->evaluateSequence(0, 2)),
           DoubleNear(0.6666667, 1e-4));
  ASSERT_THAT(
    DOUBLE(sbsw->standardEvaluator({1, 0})->evaluateSequence(0, 2)),
           DoubleNear(0.0009999, 1e-4));
  ASSERT_THAT(
    DOUBLE(sbsw->standardEvaluator({1, 1})->evaluateSequence(0, 2)),
           DoubleNear(0.3333341, 1e-4));
  ASSERT_THAT(
    DOUBLE(sbsw->standardEvaluator({1, 0, 1})->evaluateSequence(0, 3)),
           DoubleNear(0.0009999, 1e-4));
  ASSERT_THAT(
    DOUBLE(sbsw->standardEvaluator({1, 0, 1, 0})->evaluateSequence(0, 4)),
           DoubleNear(0.0009999, 1e-4));
  ASSERT_THAT(
    DOUBLE(sbsw->standardEvaluator({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
               ->evaluateSequence(0, 13)),
           DoubleNear(0.0009999, 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST_F(ASBSW, ShouldEvaluateASequenceWithPrefixSumArray) {
  ASSERT_THAT(DOUBLE(sbsw->standardEvaluator({0}, true)
                         ->evaluateSequence(0, 1)),
              DoubleNear(DOUBLE(sbsw->standardEvaluator({0})
                                    ->evaluateSequence(0, 1)), 1e-4));

  ASSERT_THAT(DOUBLE(sbsw->standardEvaluator({0, 1}, true)
                         ->evaluateSequence(0, 2)),
              DoubleNear(DOUBLE(sbsw->standardEvaluator({0, 1})
                                    ->evaluateSequence(0, 2)), 1e-4));

  ASSERT_THAT(DOUBLE(sbsw->standardEvaluator({1, 0, 1}, true)
                         ->evaluateSequence(0, 3)),
              DoubleNear(DOUBLE(sbsw->standardEvaluator({1, 0, 1})
                                    ->evaluateSequence(0, 3)), 1e-4));

  ASSERT_THAT(DOUBLE(sbsw->standardEvaluator({1, 0, 1, 0}, true)
                         ->evaluateSequence(0, 4)),
              DoubleNear(DOUBLE(sbsw->standardEvaluator({1, 0, 1, 0})
                                    ->evaluateSequence(0, 4)), 1e-4));

  ASSERT_THAT(
    DOUBLE(
      sbsw->standardEvaluator({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, true)
          ->evaluateSequence(0, 13)),
    DoubleNear(
      DOUBLE(sbsw->standardEvaluator({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
                 ->evaluateSequence(0, 13)), 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST_F(ASBSW, ShouldChooseSequenceWithDefaultSeed) {
  // TODO(igorbonadio): check bigger sequence
  ASSERT_THAT(sbsw->standardGenerator()->drawSequence(5),
              ContainerEq(Sequence(5, INVALID_SYMBOL)));
}

/*----------------------------------------------------------------------------*/
