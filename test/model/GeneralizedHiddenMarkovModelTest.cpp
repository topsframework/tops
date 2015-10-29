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

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/GeneralizedHiddenMarkovModel.hpp"
#include "model/Probability.hpp"
#include "model/Sequence.hpp"
#include "model/Matrix.hpp"
#include "model/Util.hpp"

#include "model/GHMMSignalDurationState.hpp"
#include "model/GHMMExplicitDurationState.hpp"
#include "model/GHMMGeometricDurationState.hpp"

#include "helper/DiscreteIIDModel.hpp"
#include "helper/VariableLengthMarkovChain.hpp"
#include "helper/Sequence.hpp"
#include "helper/SExprTranslator.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::ContainerEq;

using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::GeneralizedHiddenMarkovModel;
using tops::model::GeneralizedHiddenMarkovModelPtr;

using tops::model::GeneralizedHiddenMarkovModelState;
using tops::model::GeneralizedHiddenMarkovModelStatePtr;
using tops::model::GHMMSignalDurationState;
using tops::model::GHMMSignalDurationStatePtr;
using tops::model::GHMMExplicitDurationState;
using tops::model::GHMMExplicitDurationStatePtr;
using tops::model::GHMMGeometricDurationState;
using tops::model::GHMMGeometricDurationStatePtr;

using tops::model::Probability;
using tops::model::Sequence;
using tops::model::Matrix;
using tops::model::Labeler;
using tops::model::log_sum;
using tops::model::Labeling;

using tops::helper::createMachlerVLMC;
using tops::helper::createVLMCMC;
using tops::helper::createFairCoinIIDModel;
using tops::helper::generateAllCombinationsOfSymbols;

using tops::helper::SExprTranslator;

class AGHMM : public testing::Test {
 protected:
  GHMMSignalDurationStatePtr signal_duration_state
    = GHMMSignalDurationState::make(
      1, createVLMCMC(),
      DiscreteIIDModel::make(std::vector<Probability>{
        log(0.1), -std::numeric_limits<double>::infinity(), log(0.9)
      }),
      3
    );

  GHMMExplicitDurationStatePtr explicit_duration_state
    = GHMMExplicitDurationState::make(
      2, createFairCoinIIDModel(),
      DiscreteIIDModel::make(std::vector<Probability>{
        0, -std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity()
      }),
      DiscreteIIDModel::make(std::vector<Probability>{
        log(0.1), log(0.1), log(0.1), log(0.1),
        log(0.1), log(0.1), log(0.3), log(0.1)
      })
    );

  GHMMGeometricDurationStatePtr geometric_duration_state
    = GHMMGeometricDurationState::make(
      0, createMachlerVLMC(),
      DiscreteIIDModel::make(std::vector<Probability>{
        log(0.3), log(0.3), log(0.4)
      })
    );

  GeneralizedHiddenMarkovModelPtr ghmm
    = GeneralizedHiddenMarkovModel::make(
      std::vector<GeneralizedHiddenMarkovModelStatePtr>{
        geometric_duration_state,
        signal_duration_state,
        explicit_duration_state
      },
      DiscreteIIDModel::make(std::vector<Probability>{
        0, -std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity()
      }),
      3, 2
    );

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

TEST_F(AGHMM, ShouldBeSExprSerialized) {
  auto translator = SExprTranslator::make();
  auto serializer = ghmm->serializer(translator);
  serializer->serialize();
  ASSERT_EQ(
    "(GeneralizedHiddenMarkovModel: "
      "(GHMMGeometricDurationState: "
        " " /* VLMC serializarion not implemented */
        "(DiscreteIIDModel: -1.203973 -1.203973 -0.916291)) "
      "(GHMMSignalDurationState: "
        " " /* VLMC serializarion not implemented */
        "(DiscreteIIDModel: -2.302585 -inf -0.105361)) "
      "(GHMMExplicitDurationState: "
        "(DiscreteIIDModel: -0.693147 -0.693147) "
        "(DiscreteIIDModel: 0.000000 -inf -inf)))",
    translator->sexpr());
}

TEST_F(AGHMM, ShouldEvaluateSequence) {
  Sequence observation {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
  Sequence label       {0, 0, 0, 0, 1, 1, 1, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0};
  auto evaluator = ghmm->labelingEvaluator(Labeling<Sequence>(observation, label));

  ASSERT_THAT(evaluator->evaluateSequence(0, 21), DoubleNear(-35.4276, 1e-4));
}

TEST_F(AGHMM, ShouldFindBestPathUsingViterbiDecodingWithoutCache) {
  Sequence observation { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
  Sequence label       { 0, 2, 2, 2, 2, 2, 2, 2, 0, 1, 1, 1, 2, 2, 2, 2, 2, 0, 1, 1, 1 };
  // TODO(igorbonadio): check if it is correct

  auto labeler = ghmm->labeler(observation);
  auto estimation = labeler->labeling(Labeler::method::bestPath);
  auto labeling = estimation.estimated();

  ASSERT_THAT(labeling.label(), ContainerEq(label));
}

TEST_F(AGHMM, ShouldFindBestPathUsingViterbiDecodingWithCache) {
  Sequence observation { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
  Sequence label       { 0, 2, 2, 2, 2, 2, 2, 2, 0, 1, 1, 1, 2, 2, 2, 2, 2, 0, 1, 1, 1 };
  // TODO(igorbonadio): check if it is correct

  auto labeler = ghmm->labeler(observation, true);
  auto estimation = labeler->labeling(Labeler::method::bestPath);
  auto labeling = estimation.estimated();

  ASSERT_THAT(labeling.label(), ContainerEq(label));
}

TEST_F(AGHMM, ShouldFindBestPathUsingPosteriorDecodingWithoutCache) {
  Sequence observation { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
  Sequence label       { 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 2, 0, 1 };
  // TODO(igorbonadio): check if it is correct

  auto labeler = ghmm->labeler(observation);
  auto estimation = labeler->labeling(Labeler::method::posteriorDecoding);
  auto labeling = estimation.estimated();

  ASSERT_THAT(labeling.label(), ContainerEq(label));
}

TEST_F(AGHMM, ShouldFindBestPathUsingPosteriorDecodingWithCache) {
  Sequence observation { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
  Sequence label       { 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 2, 0, 1 };
  // TODO(igorbonadio): check if it is correct

  auto labeler = ghmm->labeler(observation, true);
  auto estimation = labeler->labeling(Labeler::method::posteriorDecoding);
  auto labeling = estimation.estimated();

  ASSERT_THAT(labeling.label(), ContainerEq(label));
}

TEST_F(AGHMM, ShouldReturnTheSameValueForTheForwardAndBackwardAlgorithms) {
  Matrix alpha, beta;
  Sequence sequence = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
  ASSERT_THAT(ghmm->forward(sequence, alpha),
              DoubleNear(ghmm->backward(sequence, beta), 1e-4));
}
