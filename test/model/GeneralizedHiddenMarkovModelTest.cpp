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
#include "model/Util.hpp"
#include "model/Matrix.hpp"
#include "model/Sequence.hpp"
#include "model/Probability.hpp"
#include "model/SignalDuration.hpp"
#include "model/ExplicitDuration.hpp"
#include "model/GeometricDuration.hpp"

#include "helper/Sequence.hpp"
#include "helper/SExprTranslator.hpp"
#include "helper/DiscreteIIDModel.hpp"
#include "helper/VariableLengthMarkovChain.hpp"

// Tested header
#include "model/GeneralizedHiddenMarkovModel.hpp"

// Macros
#define DOUBLE(X) static_cast<double>(X)

/*----------------------------------------------------------------------------*/
/*                             USING DECLARATIONS                             */
/*----------------------------------------------------------------------------*/

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::ContainerEq;

using tops::model::Matrix;
using tops::model::Labeler;
using tops::model::log_sum;
using tops::model::Labeling;
using tops::model::Sequence;
using tops::model::Calculator;
using tops::model::Probability;
using tops::model::SignalDuration;
using tops::model::DiscreteIIDModel;
using tops::model::ExplicitDuration;
using tops::model::GeometricDuration;
using tops::model::DiscreteIIDModelPtr;
using tops::model::VariableLengthMarkovChain;
using tops::model::GeneralizedHiddenMarkovModel;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::GeneralizedHiddenMarkovModelPtr;

using tops::helper::createVLMCMC;
using tops::helper::createMachlerVLMC;
using tops::helper::createFairCoinIIDModel;
using tops::helper::generateAllCombinationsOfSymbols;

using tops::helper::SExprTranslator;

/*----------------------------------------------------------------------------*/
/*                                  ALIASES                                   */
/*----------------------------------------------------------------------------*/

using GHMM = GeneralizedHiddenMarkovModel;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class AGHMM : public testing::Test {
 protected:
  GHMM::StatePtr signal_duration_state
    = GHMM::State::make(
      1, createVLMCMC(),
      DiscreteIIDModel::make(std::vector<Probability>{{ 0.1, 0.0, 0.9 }}),
      SignalDuration::make(3));

  GHMM::StatePtr explicit_duration_state
    = GHMM::State::make(
      2, createFairCoinIIDModel(),
      DiscreteIIDModel::make(std::vector<Probability>{{ 1.0, 0.0, 0.0 }}),
      ExplicitDuration::make(
        DiscreteIIDModel::make(std::vector<Probability>{{
          0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.3, 0.1 }})));

  DiscreteIIDModelPtr geometric_transition
    = DiscreteIIDModel::make(std::vector<Probability>{{ 0.3, 0.3, 0.4 }});

  GHMM::StatePtr geometric_duration_state
    = GHMM::State::make(
      0, createMachlerVLMC(), geometric_transition,
      GeometricDuration::make(0, geometric_transition));

  GeneralizedHiddenMarkovModelPtr ghmm
    = GeneralizedHiddenMarkovModel::make(
      std::vector<GeneralizedHiddenMarkovModel::StatePtr>{
        geometric_duration_state,
        signal_duration_state,
        explicit_duration_state },
      DiscreteIIDModel::make(std::vector<Probability>{{ 1.0, 0.0, 0.0 }}),
      3, 2);

  virtual void SetUp() {
    geometric_duration_state->addSuccessor(0);
    geometric_duration_state->addSuccessor(1);
    geometric_duration_state->addSuccessor(2);
    geometric_duration_state->addPredecessor(0);
    geometric_duration_state->addPredecessor(1);
    geometric_duration_state->addPredecessor(2);

    signal_duration_state->addSuccessor(0);
    signal_duration_state->addSuccessor(2);
    signal_duration_state->addPredecessor(0);

    explicit_duration_state->addSuccessor(0);
    explicit_duration_state->addPredecessor(0);
    explicit_duration_state->addPredecessor(1);
  }
};

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(AGHMM, ShouldBeSExprSerialized) {
  auto translator = SExprTranslator::make();
  auto serializer = ghmm->serializer(translator);
  serializer->serialize();
  ASSERT_EQ(
    "(GeneralizedHiddenMarkovModel: "
      "(GHMM::State: "
        " " /* VLMC serializarion not implemented */
        "(DiscreteIIDModel: 0.300000 0.300000 0.400000) "
        "(GeometricDuration: maximumDuration = 1)) "
      "(GHMM::State: "
        " " /* VLMC serializarion not implemented */
        "(DiscreteIIDModel: 0.100000 0.000000 0.900000) "
        "(SignalDuration: maximumDuration = 3)) "
      "(GHMM::State: "
        "(DiscreteIIDModel: 0.500000 0.500000) "
        "(DiscreteIIDModel: 1.000000 0.000000 0.000000) "
        "(ExplicitDuration: maximumDuration = 0)))",
    translator->sexpr());
}

/*----------------------------------------------------------------------------*/

TEST_F(AGHMM, ShouldEvaluateASequence) {
  Sequence observation {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
  Sequence label {
    0, 0, 0, 0, 1, 1, 1, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0};

  auto evaluator
    = ghmm->labelingEvaluator(Labeling<Sequence>(observation, label));

  ASSERT_THAT(DOUBLE(evaluator->evaluateSequence(0, 21)),
              DoubleNear(6.29856e-20, 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST_F(AGHMM, ShouldFindBestPathUsingViterbiDecodingWithoutCache) {
  Sequence observation {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
  Sequence label {
    0, 2, 2, 2, 2, 2, 2, 2, 0, 1, 1, 1, 2, 2, 2, 2, 2, 0, 1, 1, 1 };

  // TODO(igorbonadio): check if it is correct

  auto labeler = ghmm->labeler(observation);
  auto estimation = labeler->labeling(Labeler::method::bestPath);
  auto labeling = estimation.estimated();

  ASSERT_THAT(labeling.label(), ContainerEq(label));
}

/*----------------------------------------------------------------------------*/

TEST_F(AGHMM, ShouldFindBestPathUsingViterbiDecodingWithCache) {
  Sequence observation {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
  Sequence label {
    0, 2, 2, 2, 2, 2, 2, 2, 0, 1, 1, 1, 2, 2, 2, 2, 2, 0, 1, 1, 1 };

  // TODO(igorbonadio): check if it is correct

  auto labeler = ghmm->labeler(observation, true);
  auto estimation = labeler->labeling(Labeler::method::bestPath);
  auto labeling = estimation.estimated();

  ASSERT_THAT(labeling.label(), ContainerEq(label));
}

/*----------------------------------------------------------------------------*/

TEST_F(AGHMM, ShouldFindBestPathUsingPosteriorDecodingWithoutCache) {
  Sequence observation {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
  Sequence label {
    0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 2, 0, 1 };

  // TODO(igorbonadio): check if it is correct

  auto labeler = ghmm->labeler(observation);
  auto estimation = labeler->labeling(Labeler::method::posteriorDecoding);
  auto labeling = estimation.estimated();

  ASSERT_THAT(labeling.label(), ContainerEq(label));
}

/*----------------------------------------------------------------------------*/

TEST_F(AGHMM, ShouldFindBestPathUsingPosteriorDecodingWithCache) {
  Sequence observation {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
  Sequence label {
    0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 2, 0, 1 };

  // TODO(igorbonadio): check if it is correct

  auto labeler = ghmm->labeler(observation, true);
  auto estimation = labeler->labeling(Labeler::method::posteriorDecoding);
  auto labeling = estimation.estimated();

  ASSERT_THAT(labeling.label(), ContainerEq(label));
}

/*----------------------------------------------------------------------------*/

TEST_F(AGHMM, ShouldReturnTheSameValueForTheForwardAndBackwardAlgorithms) {
  Matrix alpha, beta;
  Sequence sequence {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };

  auto calculator = ghmm->calculator(sequence);

  ASSERT_THAT(
    DOUBLE(calculator->calculate(Calculator::direction::forward)),
    DoubleNear(calculator->calculate(Calculator::direction::backward), 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST_F(AGHMM,
    ReturnsTheSameValueForTheForwardAndBackwardAlgorithmsWithCache) {
  Matrix alpha, beta;
  Sequence sequence {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };

  auto calculator = ghmm->calculator(sequence, true);

  ASSERT_THAT(
    DOUBLE(calculator->calculate(Calculator::direction::forward)),
    DoubleNear(calculator->calculate(Calculator::direction::backward), 1e-4));
}

/*----------------------------------------------------------------------------*/
