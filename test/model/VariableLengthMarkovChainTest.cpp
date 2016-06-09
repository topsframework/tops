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
#include <math.h>
#include <vector>

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/Sequence.hpp"
#include "model/DiscreteIIDModel.hpp"

#include "helper/Sequence.hpp"

// Tested header
#include "model/VariableLengthMarkovChain.hpp"
#include "helper/VariableLengthMarkovChain.hpp"

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
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::ProbabilisticModelPtr;
using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;

using tops::helper::createMachlerVLMC;
using tops::helper::generateRandomSequence;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class AVLMC : public testing::Test {
 protected:
  VariableLengthMarkovChainPtr vlmc = createMachlerVLMC();
};

/*----------------------------------------------------------------------------*/
/*                                SIMPLE TESTS                                */
/*----------------------------------------------------------------------------*/

TEST(VLMC, ShouldBeTrainedUsingContextAlgorithm) {
  auto vlmc_trainer = VariableLengthMarkovChain::standardTrainer();

  vlmc_trainer->add_training_set({{1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                  {1, 1, 0, 1, 0, 1, 1, 0, 1, 0},
                                  {0, 1, 1, 0, 0, 0, 0, 1, 0, 1}});

  auto vlmc = vlmc_trainer->train(
    VariableLengthMarkovChain::context_algorithm{}, 2, 0.1);

  ASSERT_THAT(DOUBLE(vlmc->standardEvaluator({1, 0, 1, 0})
                         ->evaluateSequence(0, 4)),
              DoubleNear(0.062499, 1e-4));
  ASSERT_THAT(DOUBLE(vlmc->standardEvaluator({0, 0, 0, 1, 1, 1, 1})
                         ->evaluateSequence(0, 7)),
              DoubleNear(0.007812, 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST(VLMC, ShouldBeTrainedUsingFixedLengthMarkovChainAlgorithm) {
  auto vlmc_trainer = VariableLengthMarkovChain::standardTrainer();

  vlmc_trainer->add_training_set({{1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                  {1, 1, 0, 1, 0, 1, 1, 0, 1, 0},
                                  {0, 1, 1, 0, 0, 0, 0, 1, 0, 1}});

  auto vlmc = vlmc_trainer->train(
    VariableLengthMarkovChain::fixed_length_algorithm{},
    2, 2, 1.5, std::vector<double>{1.0, 1.0, 1.0, 1.0}, nullptr);

  ASSERT_THAT(DOUBLE(vlmc->standardEvaluator({1, 0, 1, 0})
                         ->evaluateSequence(0, 4)),
              DoubleNear(0.253510, 1e-4));
  ASSERT_THAT(DOUBLE(vlmc->standardEvaluator({1, 1, 1, 1})
                         ->evaluateSequence(0, 4)),
              DoubleNear(0.005427, 1e-4));
  ASSERT_THAT(DOUBLE(vlmc->standardEvaluator({0, 0, 0, 1, 1, 1, 1})
                         ->evaluateSequence(0, 7)),
              DoubleNear(0.000416, 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST(VLMC, ShouldBeTrainedUsingInterpolatedMarkovChainAlgorithm) {
  auto vlmc_trainer = VariableLengthMarkovChain::standardTrainer();

  vlmc_trainer->add_training_set({{1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                  {1, 1, 0, 1, 0, 1, 1, 0, 1, 0},
                                  {0, 1, 1, 0, 0, 0, 0, 1, 0, 1}});

  auto vlmc = vlmc_trainer->train(
    VariableLengthMarkovChain::interpolation_algorithm{},
    std::vector<double>{1.0, 1.0, 1.0, 1.0}, 2, 2, 1.5, nullptr);

  ASSERT_THAT(DOUBLE(vlmc->standardEvaluator({1, 0, 1, 0})
                         ->evaluateSequence(0, 4)),
              DoubleNear(0.062092, 1e-4));
  ASSERT_THAT(DOUBLE(vlmc->standardEvaluator({1, 1, 1, 1})
                         ->evaluateSequence(0, 4)),
              DoubleNear(0.049393, 1e-4));
  ASSERT_THAT(DOUBLE(vlmc->standardEvaluator({0, 0, 0, 1, 1, 1, 1})
                         ->evaluateSequence(0, 7)),
              DoubleNear(0.0072941, 1e-4));
}

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(AVLMC, ShouldEvaluateAPosition) {
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({0})->evaluateSequence(0, 1)),
    DoubleEq(0.50));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({1})->evaluateSequence(0, 1)),
    DoubleEq(0.50));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({0, 1})->evaluateSequence(1, 2)),
    DoubleEq(0.80));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({0, 0})->evaluateSequence(1, 2)),
    DoubleEq(0.20));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({1, 0})->evaluateSequence(1, 2)),
    DoubleEq(0.21));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({1, 1})->evaluateSequence(1, 2)),
    DoubleEq(0.79));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({1, 0, 1})->evaluateSequence(2, 3)),
    DoubleEq(0.80));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({1, 0, 1, 0})->evaluateSequence(3, 4)),
    DoubleEq(0.10));
}

/*----------------------------------------------------------------------------*/

TEST_F(AVLMC, ShouldEvaluateASequence) {
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({0})->evaluateSequence(0, 1)),
    DoubleEq(0.50));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({1})->evaluateSequence(0, 1)),
    DoubleEq(0.50));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({0, 1})->evaluateSequence(0, 2)),
    DoubleEq(0.50 * 0.80));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({0, 0})->evaluateSequence(0, 2)),
    DoubleEq(0.50 * 0.20));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({1, 0})->evaluateSequence(0, 2)),
    DoubleEq(0.50 * 0.21));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({1, 1})->evaluateSequence(0, 2)),
    DoubleEq(0.50 * 0.79));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({1, 0, 1})->evaluateSequence(0, 3)),
    DoubleEq(0.50 * 0.21 * 0.80));
  ASSERT_THAT(
    DOUBLE(vlmc->standardEvaluator({1, 0, 1, 0})->evaluateSequence(0, 4)),
    DoubleEq(0.50 * 0.21 * 0.80 * 0.10));
}

/*----------------------------------------------------------------------------*/

TEST_F(AVLMC, ShouldEvaluateASequenceWithPrefixSumArray) {
  for (int i = 1; i < 1000; i++) {
    auto data = generateRandomSequence(i, 2);
    auto size = data.size();
    ASSERT_THAT(
      DOUBLE(vlmc->standardEvaluator(data, true)->evaluateSequence(0, size)),
      DoubleEq(DOUBLE(vlmc->standardEvaluator(data)
                          ->evaluateSequence(0, size))));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AVLMC, ShouldChooseSequenceWithDefaultSeed) {
  // TODO(igorbonadio): check bigger sequence
  ASSERT_THAT(vlmc->standardGenerator()->drawSequence(5),
              ContainerEq(Sequence{0, 1, 1, 0, 1}));
}

/*----------------------------------------------------------------------------*/
