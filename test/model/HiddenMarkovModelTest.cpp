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
#include <vector>
#include <iostream>

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/HiddenMarkovModel.hpp"
#include "model/Sequence.hpp"
#include "model/Util.hpp"

#include "helper/HiddenMarkovModel.hpp"
#include "helper/Sequence.hpp"

using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::Eq;
using ::testing::ContainerEq;

using tops::model::HiddenMarkovModel;
using tops::model::HiddenMarkovModelPtr;
using tops::model::Sequence;
using tops::model::Labeling;
using tops::model::Matrix;
using tops::model::log_sum;
using tops::model::INVALID_SYMBOL;

using tops::helper::createDishonestCoinCasinoHMM;
using tops::helper::generateAllCombinationsOfSymbols;

class AHiddenMarkovModel : public testing::Test {
 protected:
  HiddenMarkovModelPtr hmm = createDishonestCoinCasinoHMM();
};

TEST_F(AHiddenMarkovModel, ShouldEvaluateTheJointProbability) {
  ASSERT_THAT(hmm->decodableEvaluator({0, 0, 1})->probabilityOf({0, 1, 1}, 0, 3),
              DoubleEq(log(0.9) + log(0.5) +
                       log(0.3) + log(0.2) +
                       log(0.5) + log(0.8)));
}

TEST_F(AHiddenMarkovModel, FindsTheBestPath) {
  std::vector<std::vector<Sequence>> test_set = {
    {{0},{0}},
    {{1},{0}},
    {{0, 0, 0},{0, 0, 0}},
    {{1, 1, 1, 1, 1, 1},{0, 1, 1, 1, 1, 1}}
  };
  for(auto test : test_set) {
    auto evaluator = hmm->decodableEvaluator(test[0]);
    auto labeling = evaluator->labeling(Labeling::Method::bestPath);

    ASSERT_THAT(labeling.probability(),
                DoubleEq(evaluator->probabilityOf(test[1], 0, test[1].size())));
    ASSERT_THAT(labeling.sequence(), Eq(test[1]));
  }
}

TEST_F(AHiddenMarkovModel, CalculatesProbabilityOfObservationsUsingForward) {
  std::vector<Sequence> test_set = {
    {0},
    {1},
    {0, 0},
    {0, 1},
    {1, 0},
    {1, 1},
    {0, 0, 0},
    {0, 0, 1},
    {0, 1, 0},
    {0, 1, 1},
    {1, 0, 0},
    {1, 0, 1},
    {1, 1, 0},
    {1, 1, 1}
  };

  for (auto observations : test_set) {
    double px = -HUGE;
    auto evaluator = hmm->decodableEvaluator(observations);

    std::vector<Sequence> labels = generateAllCombinationsOfSymbols(observations.size());
    for (auto y : labels) {
      px = log_sum(px, evaluator->probabilityOf(y, 0, observations.size()));
    }
    ASSERT_THAT(evaluator->probabilityOf(0, observations.size()), DoubleEq(px));
  }
}

TEST_F(AHiddenMarkovModel, DecodesASequenceOfObservationsUsingThePosteriorProbability) {
  std::vector<std::vector<Sequence>> test_set = {
    {{0},{0}},
    {{1},{0}},
    {{0, 0, 0},{0, 0, 0}},
    {{1, 1, 1, 1, 1, 1},{0, 0, 1, 1, 1, 1}}
  };

  for(auto test : test_set) {
    auto evaluator = hmm->decodableEvaluator(test[0]);
    auto labeling = evaluator->labeling(Labeling::Method::posteriorDecoding);

    ASSERT_THAT(labeling.probability(),
                DoubleEq(evaluator->probabilityOf(test[1], 0, test[1].size())));
    ASSERT_THAT(labeling.sequence(), Eq(test[1]));
  }
}

TEST(HiddenMarkovModel, ShouldBeTrainedUsingMLAlgorithm) {
  std::vector<Sequence> observation_training_set = {
    {0, 0, 0, 1, 1},
    {0, 0, 0, 1, 0, 0, 1, 1},
    {0, 0, 0, 1, 1, 0, 0},
  };
  std::vector<Sequence> state_training_set = {
    {1, 1, 1, 1, 1},
    {0, 1, 1, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 0, 1, 0},
  };

  auto trained_hmm = HiddenMarkovModel::trainML(
      observation_training_set, state_training_set, 2, 2, 0.1);

  auto evaluator0 = trained_hmm->decodableEvaluator({0, 0, 0});
  auto evaluator1 = trained_hmm->decodableEvaluator({1, 1, 1});

  ASSERT_THAT(evaluator0->probabilityOf({0, 0, 0}, 0, 3), DoubleNear(-2.32992, 1e-4));
  ASSERT_THAT(evaluator0->probabilityOf({1, 1, 1}, 0, 3), DoubleNear(-3.20183, 1e-4));
  ASSERT_THAT(evaluator1->probabilityOf({1, 1, 1}, 0, 3), DoubleNear(-4.39373, 1e-4));
  ASSERT_THAT(evaluator1->probabilityOf({0, 0, 0}, 0, 3), DoubleNear(-4.81600, 1e-4));
}

TEST_F(AHiddenMarkovModel, ShouldBeTrainedUsingBaumWelchAlgorithm) {
  std::vector<Sequence> observation_training_set = {
    {0, 0, 0, 1, 1},
    {0, 0, 0, 1, 0, 0, 1, 1},
    {0, 0, 0, 1, 1, 0, 0},
  };

  auto trained_hmm = HiddenMarkovModel::trainBaumWelch(
      observation_training_set, hmm, 500, 1e-4);

  auto evaluator0 = trained_hmm->decodableEvaluator({0, 0, 0});
  auto evaluator1 = trained_hmm->decodableEvaluator({1, 1, 1});

  ASSERT_THAT(evaluator0->probabilityOf({0, 0, 0}, 0, 3), DoubleNear(-1.65545, 1e-4));
  ASSERT_THAT(evaluator0->probabilityOf({1, 1, 1}, 0, 3), DoubleNear(-311.83440, 1e-4));
  ASSERT_THAT(evaluator1->probabilityOf({1, 1, 1}, 0, 3), DoubleNear(-313.26651, 1e-4));
  ASSERT_THAT(evaluator1->probabilityOf({0, 0, 0}, 0, 3), DoubleNear(-110.38680, 1e-4));
}

TEST_F(AHiddenMarkovModel, ShouldChooseSequenceWithSeed42) {
  // TODO(igorbonadio): implement method
  ASSERT_THAT(hmm->generator()->choose(5), ContainerEq(Sequence(5, INVALID_SYMBOL)));
}
