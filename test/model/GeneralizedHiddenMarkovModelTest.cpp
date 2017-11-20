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

#include "exception/NotYetImplemented.hpp"

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

using tops::exception::NotYetImplemented;

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
  DiscreteIIDModelPtr geometric_transition
    = DiscreteIIDModel::make(std::vector<Probability>{{ 0.3, 0.3, 0.4 }});

  DiscreteIIDModelPtr signal_transition
    = DiscreteIIDModel::make(std::vector<Probability>{{ 0.1, 0.0, 0.9 }});

  DiscreteIIDModelPtr explicit_transition
    = DiscreteIIDModel::make(std::vector<Probability>{{ 1.0, 0.0, 0.0 }});

  DiscreteIIDModelPtr explicit_duration
    = DiscreteIIDModel::make(
        std::vector<Probability>{{ 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.3, 0.1 }});

  std::vector<GHMM::StatePtr> states {
    GHMM::State::make( // Geometric duration state
      /* id         */ 0,
      /* emission   */ createMachlerVLMC(),
      /* transition */ geometric_transition,
      /* duration   */ GeometricDuration::make(0, geometric_transition)),

    GHMM::State::make( // Signal duration state
      /* id         */ 1,
      /* emission   */ createVLMCMC(),
      /* transition */ signal_transition,
      /* duration   */ SignalDuration::make(3)),

    GHMM::State::make( // Explicit duration state
      /* id         */ 2,
      /* emission   */ createFairCoinIIDModel(),
      /* transition */ explicit_transition,
      /* duration   */ ExplicitDuration::make(explicit_duration)),
  };

  DiscreteIIDModelPtr initial_probabilities
    = DiscreteIIDModel::make(std::vector<Probability>{{ 1.0, 0.0, 0.0 }});

  GeneralizedHiddenMarkovModelPtr ghmm
    = GeneralizedHiddenMarkovModel::make(states, initial_probabilities, 3, 2);

  virtual void SetUp() {
    //  .-.
    //  | v
    //  (0) <--> (2)
    //   ^        ^
    //    `.    .´
    //      v .´
    //      (1)
    //
    // Graph of states

    states[0]->addSuccessor(0);
    states[0]->addSuccessor(1);
    states[0]->addSuccessor(2);
    states[0]->addPredecessor(0);
    states[0]->addPredecessor(1);
    states[0]->addPredecessor(2);

    states[1]->addSuccessor(0);
    states[1]->addSuccessor(2);
    states[1]->addPredecessor(0);

    states[2]->addSuccessor(0);
    states[2]->addPredecessor(0);
    states[2]->addPredecessor(1);
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
    = ghmm->labelingEvaluator(Labeling<Sequence>{observation, {}, label});

  ASSERT_THAT(DOUBLE(evaluator->evaluateSequence(0, 21)),
              DoubleNear(6.29856e-20, 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST_F(AGHMM, ShouldThrowAnNotYetImplementedInEvaluateSequence) {
  Sequence observation {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};

  auto evaluator = ghmm->standardEvaluator(observation, true);

  ASSERT_THROW(evaluator->evaluateSequence(0, 0), NotYetImplemented);

  evaluator = ghmm->standardEvaluator(observation);

  ASSERT_THROW(evaluator->evaluateSequence(0, 0), NotYetImplemented);
}

/*----------------------------------------------------------------------------*/

TEST_F(AGHMM, ShouldThrowAnNotYetImplementedInEvaluateSymbol) {
  Sequence observation {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};

  auto evaluator = ghmm->standardEvaluator(observation, true);

  ASSERT_THROW(evaluator->evaluateSymbol(0), NotYetImplemented);

  evaluator = ghmm->standardEvaluator(observation);

  ASSERT_THROW(evaluator->evaluateSymbol(0), NotYetImplemented);
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

  ASSERT_THAT(labeling.label, ContainerEq(label));
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

  ASSERT_THAT(labeling.label, ContainerEq(label));
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

  ASSERT_THAT(labeling.label, ContainerEq(label));
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

  ASSERT_THAT(labeling.label, ContainerEq(label));
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
