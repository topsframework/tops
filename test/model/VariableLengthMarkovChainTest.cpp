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
#include "model/VariableLengthMarkovChain.hpp"
#include "model/DiscreteIIDModel.hpp"
#include "model/Sequence.hpp"

#include "helper/VariableLengthMarkovChain.hpp"
#include "helper/Sequence.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::ContainerEq;

using tops::model::ProbabilisticModelPtr;
using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::Sequence;

using tops::helper::createMachlerVLMC;
using tops::helper::generateRandomSequence;

class AVLMC : public testing::Test {
 protected:
  VariableLengthMarkovChainPtr vlmc = createMachlerVLMC();
};

TEST_F(AVLMC, ShouldEvaluateAPosition) {
  ASSERT_THAT(vlmc->standardEvaluator({0})->evaluateSequence(0, 1), DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->standardEvaluator({1})->evaluateSequence(0, 1), DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->standardEvaluator({0, 1})->evaluateSequence(1, 2), DoubleEq(log(0.80)));
  ASSERT_THAT(vlmc->standardEvaluator({0, 0})->evaluateSequence(1, 2), DoubleEq(log(0.20)));
  ASSERT_THAT(vlmc->standardEvaluator({1, 0})->evaluateSequence(1, 2), DoubleEq(log(0.21)));
  ASSERT_THAT(vlmc->standardEvaluator({1, 1})->evaluateSequence(1, 2), DoubleEq(log(0.79)));
  ASSERT_THAT(vlmc->standardEvaluator({1, 0, 1})->evaluateSequence(2, 3), DoubleEq(log(0.80)));
  ASSERT_THAT(vlmc->standardEvaluator({1, 0, 1, 0})->evaluateSequence(3, 4), DoubleEq(log(0.10)));
}

TEST_F(AVLMC, ShouldEvaluateASequence) {
  ASSERT_THAT(vlmc->standardEvaluator({0})->evaluateSequence(0, 1),
              DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->standardEvaluator({1})->evaluateSequence(0, 1),
              DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->standardEvaluator({0, 1})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.80)));
  ASSERT_THAT(vlmc->standardEvaluator({0, 0})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.20)));
  ASSERT_THAT(vlmc->standardEvaluator({1, 0})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.21)));
  ASSERT_THAT(vlmc->standardEvaluator({1, 1})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.79)));
  ASSERT_THAT(vlmc->standardEvaluator({1, 0, 1})->evaluateSequence(0, 3),
              DoubleEq(log(0.50) + log(0.21) + log(0.80)));
  ASSERT_THAT(vlmc->standardEvaluator({1, 0, 1, 0})->evaluateSequence(0, 4),
              DoubleEq(log(0.50) + log(0.21) + log(0.80) + log(0.10)));
}

TEST_F(AVLMC, ShouldEvaluateASequenceWithPrefixSumArray) {
  for (int i = 1; i < 1000; i++) {
    auto data = generateRandomSequence(i, 2);
    ASSERT_THAT(vlmc->standardEvaluator(data, true)->evaluateSequence(0, data.size()),
                DoubleEq(vlmc->standardEvaluator(data)->evaluateSequence(0, data.size())));
  }
}

TEST_F(AVLMC, ShouldChooseSequenceWithSeed42) {
  // TODO(igorbonadio): check bigger sequence
  ASSERT_THAT(vlmc->standardGenerator()->drawSequence(5), ContainerEq(Sequence{0, 1, 1, 0, 1}));
}

TEST(VLMC, ShouldBeTrainedUsingContextAlgorithm) {
  auto vlmc_trainer = VariableLengthMarkovChain::standardTrainer();

  vlmc_trainer->add_training_set({{1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                  {1, 1, 0, 1, 0, 1, 1, 0, 1, 0},
                                  {0, 1, 1, 0, 0, 0, 0, 1, 0, 1}});

  auto vlmc = vlmc_trainer->train(
    VariableLengthMarkovChain::context_algorithm{}, 2, 0.1);

  ASSERT_THAT(vlmc->standardEvaluator({1, 0, 1, 0})->evaluateSequence(0, 4),
              DoubleNear(-2.77259, 1e-4));
  ASSERT_THAT(vlmc->standardEvaluator({0, 0, 0, 1, 1, 1, 1})->evaluateSequence(0, 7),
              DoubleNear(-4.85203, 1e-4));
}

TEST(VLMC, ShouldBeTrainedUsingFixedLengthMarkovChainAlgorithm) {
  auto vlmc_trainer = VariableLengthMarkovChain::standardTrainer();

  vlmc_trainer->add_training_set({{1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                  {1, 1, 0, 1, 0, 1, 1, 0, 1, 0},
                                  {0, 1, 1, 0, 0, 0, 0, 1, 0, 1}});

  auto vlmc = vlmc_trainer->train(
    VariableLengthMarkovChain::fixed_length_algorithm{},
    2, 2, 1.5, std::vector<double>{1.0, 1.0, 1.0, 1.0}, nullptr);

  ASSERT_THAT(vlmc->standardEvaluator({1, 0, 1, 0})->evaluateSequence(0, 4),
              DoubleNear(-1.37235, 1e-4));
  ASSERT_THAT(vlmc->standardEvaluator({1, 1, 1, 1})->evaluateSequence(0, 4),
              DoubleNear(-5.21625, 1e-4));
  ASSERT_THAT(vlmc->standardEvaluator({0, 0, 0, 1, 1, 1, 1})->evaluateSequence(0, 7),
              DoubleNear(-7.78482, 1e-4));
}

TEST(VLMC, ShouldBeTrainedUsingInterpolatedMarkovChainAlgorithm) {
  auto vlmc_trainer = VariableLengthMarkovChain::standardTrainer();

  vlmc_trainer->add_training_set({{1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                  {1, 1, 0, 1, 0, 1, 1, 0, 1, 0},
                                  {0, 1, 1, 0, 0, 0, 0, 1, 0, 1}});

  auto vlmc = vlmc_trainer->train(
    VariableLengthMarkovChain::interpolation_algorithm{},
    std::vector<double>{1.0, 1.0, 1.0, 1.0}, 2, 2, 1.5, nullptr);

  ASSERT_THAT(vlmc->standardEvaluator({1, 0, 1, 0})->evaluateSequence(0, 4),
              DoubleNear(-2.77913, 1e-4));
  ASSERT_THAT(vlmc->standardEvaluator({1, 1, 1, 1})->evaluateSequence(0, 4),
              DoubleNear(-3.00795, 1e-4));
  ASSERT_THAT(vlmc->standardEvaluator({0, 0, 0, 1, 1, 1, 1})->evaluateSequence(0, 7),
              DoubleNear(-4.92068, 1e-4));
}
