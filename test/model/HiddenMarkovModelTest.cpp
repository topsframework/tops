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
#include <iostream>

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/Util.hpp"
#include "model/Sequence.hpp"

#include "helper/Sequence.hpp"
#include "helper/SExprTranslator.hpp"

// Tested header
#include "model/HiddenMarkovModel.hpp"
#include "helper/HiddenMarkovModel.hpp"

/*----------------------------------------------------------------------------*/
/*                             USING DECLARATIONS                             */
/*----------------------------------------------------------------------------*/

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::ContainerEq;

using tops::model::Matrix;
using tops::model::log_sum;
using tops::model::Labeler;
using tops::model::Labeling;
using tops::model::Sequence;
using tops::model::Calculator;
using tops::model::INVALID_SYMBOL;
using tops::model::HiddenMarkovModel;
using tops::model::HiddenMarkovModelPtr;

using tops::helper::SExprTranslator;
using tops::helper::createDishonestCoinCasinoHMM;
using tops::helper::generateAllCombinationsOfSymbols;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class AHiddenMarkovModel : public testing::Test {
 protected:
  HiddenMarkovModelPtr hmm = createDishonestCoinCasinoHMM();
};

/*----------------------------------------------------------------------------*/
/*                                SIMPLE TESTS                                */
/*----------------------------------------------------------------------------*/

TEST(HiddenMarkovModel, ShouldBeTrainedUsingMLAlgorithm) {
  auto hmm_trainer = HiddenMarkovModel::labelingTrainer();

  hmm_trainer->add_training_set({
    Labeling<Sequence>{ {0, 0, 0, 1, 1},          {1, 1, 1, 1, 1}          },
    Labeling<Sequence>{ {0, 0, 0, 1, 0, 0, 1, 1}, {0, 1, 1, 0, 0, 0, 1, 1} },
    Labeling<Sequence>{ {0, 0, 0, 1, 1, 0, 0},    {0, 0, 0, 0, 0, 1, 0}    }
  });

  auto trained_hmm = hmm_trainer->train(
    HiddenMarkovModel::maximum_likehood_algorithm{}, 2, 2, 0.1);

  std::vector<Sequence> seq { {0, 0, 0}, {1, 1, 1} };

  auto evaluator00 = trained_hmm->labelingEvaluator({ seq[0], seq[0] });
  auto evaluator01 = trained_hmm->labelingEvaluator({ seq[0], seq[1] });
  auto evaluator10 = trained_hmm->labelingEvaluator({ seq[1], seq[0] });
  auto evaluator11 = trained_hmm->labelingEvaluator({ seq[1], seq[1] });

  ASSERT_THAT(evaluator00->evaluateSequence(0, 3), DoubleNear(-2.32992, 1e-4));
  ASSERT_THAT(evaluator01->evaluateSequence(0, 3), DoubleNear(-3.20183, 1e-4));
  ASSERT_THAT(evaluator10->evaluateSequence(0, 3), DoubleNear(-4.81600, 1e-4));
  ASSERT_THAT(evaluator11->evaluateSequence(0, 3), DoubleNear(-4.39373, 1e-4));
}

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel, ShouldEvaluateTheJointProbability) {
  Sequence observation {0, 0, 1};
  Sequence label       {0, 1, 1};
  Labeling<Sequence> labeling(observation, label);

  ASSERT_THAT(hmm->labelingEvaluator(labeling)->evaluateSequence(0, 3),
              DoubleEq(log(0.9) + log(0.5) +
                       log(0.3) + log(0.2) +
                       log(0.5) + log(0.8)));
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel, ShouldEvaluateTheJointProbabilityWithCache) {
  Sequence observation {0, 0, 1};
  Sequence label       {0, 1, 1};
  Labeling<Sequence> labeling(observation, label);

  ASSERT_THAT(hmm->labelingEvaluator(labeling, true)->evaluateSequence(0, 3),
              DoubleEq(log(0.9) + log(0.5) +
                       log(0.3) + log(0.2) +
                       log(0.5) + log(0.8)));
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel, FindsTheBestPath) {
  std::vector<std::vector<Sequence>> test_set = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {0, 1, 1, 1, 1, 1}}
  };
  for (auto test : test_set) {
    auto labeler = hmm->labeler(test[0]);
    auto estimation = labeler->labeling(Labeler::method::bestPath);
    auto labeling = estimation.estimated();

    ASSERT_THAT(labeling.label(), Eq(test[1]));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel, FindsTheBestPathWithCache) {
  std::vector<std::vector<Sequence>> test_set = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {0, 1, 1, 1, 1, 1}}
  };
  for (auto test : test_set) {
    auto labeler = hmm->labeler(test[0], true);
    auto estimation = labeler->labeling(Labeler::method::bestPath);
    auto labeling = estimation.estimated();

    ASSERT_THAT(labeling.label(), Eq(test[1]));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel, CalculatesProbabilityOfObservations) {
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

  for (auto observation : test_set) {
    double px = -std::numeric_limits<double>::infinity();
    auto standardEvaluator = hmm->standardEvaluator(observation);

    std::vector<Sequence> labels
      = generateAllCombinationsOfSymbols(observation.size());

    for (auto label : labels) {
      auto labelingEvaluator = hmm->labelingEvaluator({ observation, label });
      px = log_sum(
        px, labelingEvaluator->evaluateSequence(0, observation.size()));
    }

    ASSERT_THAT(standardEvaluator->evaluateSequence(0, observation.size()),
                DoubleEq(px));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel,
    DecodesASequenceOfObservationsUsingThePosteriorProbability) {
  std::vector<std::vector<Sequence>> test_set = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {0, 0, 1, 1, 1, 1}}
  };

  for (auto test : test_set) {
    auto labeler = hmm->labeler(test[0]);
    auto estimation = labeler->labeling(Labeler::method::posteriorDecoding);
    auto labeling = estimation.estimated();

    ASSERT_THAT(labeling.label(), Eq(test[1]));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel,
    DecodesASequenceOfObservationsUsingThePosteriorProbabilityWithCache) {
  std::vector<std::vector<Sequence>> test_set = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {0, 0, 1, 1, 1, 1}}
  };

  for (auto test : test_set) {
    auto labeler = hmm->labeler(test[0]);
    auto estimation = labeler->labeling(Labeler::method::posteriorDecoding);
    auto labeling = estimation.estimated();

    ASSERT_THAT(labeling.label(), Eq(test[1]));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel, CalculatesForwardAndBackwardProbabilities) {
  std::vector<std::vector<Sequence>> test_set = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {0, 0, 1, 1, 1, 1}}
  };

  for (auto test : test_set) {
    auto calculator = hmm->calculator(test[0]);
    auto prob_f = calculator->calculate(Calculator::direction::forward);
    auto prob_b = calculator->calculate(Calculator::direction::backward);

    ASSERT_THAT(prob_b, DoubleNear(prob_f, 1e-4));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel, CalculatesForwardAndBackwardProbabilitiesWithCache) {
  std::vector<std::vector<Sequence>> test_set = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {0, 0, 1, 1, 1, 1}}
  };

  for (auto test : test_set) {
    auto calculator = hmm->calculator(test[0], true);
    auto prob_f = calculator->calculate(Calculator::direction::forward);
    auto prob_b = calculator->calculate(Calculator::direction::backward);

    ASSERT_THAT(prob_b, DoubleNear(prob_f, 1e-4));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel, ShouldBeTrainedUsingBaumWelchAlgorithm) {
  auto hmm_trainer = HiddenMarkovModel::standardTrainer();

  hmm_trainer->add_training_set({
    {0, 0, 0, 1, 1},
    {0, 0, 0, 1, 0, 0, 1, 1},
    {0, 0, 0, 1, 1, 0, 0},
  });

  auto trained_hmm = hmm_trainer->train(
    HiddenMarkovModel::baum_welch_algorithm{}, hmm, 500, 1e-4);

  std::vector<Sequence> seq { {0, 0, 0}, {1, 1, 1} };

  auto evaluator00 = trained_hmm->labelingEvaluator({ seq[0], seq[0] });
  auto evaluator01 = trained_hmm->labelingEvaluator({ seq[0], seq[1] });
  auto evaluator10 = trained_hmm->labelingEvaluator({ seq[1], seq[0] });
  auto evaluator11 = trained_hmm->labelingEvaluator({ seq[1], seq[1] });

  ASSERT_THAT(evaluator00->evaluateSequence(0, 3),
              DoubleNear(-1.65545, 1e-4));
  ASSERT_THAT(evaluator01->evaluateSequence(0, 3),
              DoubleNear(-311.83440, 1e-4));
  ASSERT_THAT(evaluator10->evaluateSequence(0, 3),
              DoubleNear(-110.38680, 1e-4));
  ASSERT_THAT(evaluator11->evaluateSequence(0, 3),
              DoubleNear(-313.26651, 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel, ShouldBeSExprSerialized) {
  auto translator = SExprTranslator::make();
  auto serializer = hmm->serializer(translator);
  serializer->serialize();
  ASSERT_EQ(translator->sexpr(),
    "(HiddenMarkovModel: "
      "(HMM::State: "
        "(DiscreteIIDModel: -0.693147 -0.693147) "
        "(DiscreteIIDModel: -0.356675 -1.203973)) "
      "(HMM::State: "
        "(DiscreteIIDModel: -1.609438 -0.223144) "
        "(DiscreteIIDModel: -0.693147 -0.693147)))");
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel, ShouldChooseSequenceWithDefaultSeed) {
  // TODO(igorbonadio): implement method
  ASSERT_THAT(hmm->standardGenerator()->drawSequence(5),
              ContainerEq(Sequence(5, INVALID_SYMBOL)));
}

/*----------------------------------------------------------------------------*/

TEST_F(AHiddenMarkovModel, ShouldChooseLabelingWithSeed42) {
  auto labeling = hmm->labelingGenerator()->drawSequence(5);
  ASSERT_THAT(labeling.observation(),
              ContainerEq(Sequence{1, 1, 1, 1, 1}));
  ASSERT_THAT(labeling.label(),
              ContainerEq(Sequence{0, 1, 0, 0, 1}));
}

/*----------------------------------------------------------------------------*/
