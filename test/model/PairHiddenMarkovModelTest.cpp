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
#include <iomanip>

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/Util.hpp"
#include "model/Sequence.hpp"
#include "model/Probability.hpp"
#include "model/RandomNumberGeneratorAdapter.hpp"

#include "exception/NotYetImplemented.hpp"

#include "helper/Sequence.hpp"
#include "helper/SExprTranslator.hpp"

// Tested header
#include "model/PairHiddenMarkovModel.hpp"
#include "helper/PairHiddenMarkovModel.hpp"

// Macros
#define DOUBLE(X) static_cast<double>(X)

/*----------------------------------------------------------------------------*/
/*                             USING DECLARATIONS                             */
/*----------------------------------------------------------------------------*/

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::ContainerEq;

using tops::model::Cube;
using tops::model::Sequence;
using tops::model::Sequences;
using tops::model::PairHiddenMarkovModel;
using tops::model::PairHiddenMarkovModelPtr;
using tops::model::RandomNumberGeneratorAdapter;

using tops::exception::NotYetImplemented;

using tops::helper::createGlobalAlignmentPairHMM;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class APairHiddenMarkovModel : public testing::Test {
 protected:
  PairHiddenMarkovModelPtr phmm = createGlobalAlignmentPairHMM();
};

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(APairHiddenMarkovModel, CalculatesForwardAndBackwardProbabilities) {
  std::vector<Sequences> tests = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}},
  };

  for (const auto& test : tests) {
    auto [ prob_f, alphas ] = phmm->forward(test);
    auto [ prob_b, betas ] = phmm->backward(test);

    /**/ std::cerr << "prob_f: " << prob_f << std::endl;
    /**/ std::cerr << "prob_b: " << prob_b << std::endl;

    EXPECT_THAT(DOUBLE(prob_f), DoubleNear(DOUBLE(prob_b), 1e-7));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APairHiddenMarkovModel, FindsTheBestPath) {
  std::vector<Sequences> tests = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}},
  };

  std::vector<PairHiddenMarkovModel::LabelerReturn> expected = {
    { 0.02   , {0, 1, 4}                , tests[0], {} },
    { 0.005  , {0, 1, 4}                , tests[1], {} },
    { 0.0008 , {0, 1, 1, 1, 4}          , tests[2], {} },
    { 6.4e-6 , {0, 1, 1, 1, 1, 1, 1, 4} , tests[3], {} },
  };

  for (unsigned int t = 0; t < tests.size(); t++) {
    auto [ estimation, label, alignment, _ ] = phmm->viterbi(tests[t]);

    /**/ std::cerr << "prob: " << estimation << std::endl;

    EXPECT_THAT(label, Eq(expected[t].label));
    EXPECT_THAT(alignment, Eq(expected[t].alignment));
    EXPECT_THAT(DOUBLE(estimation),
        DoubleNear(DOUBLE(expected[t].estimation), 1e-7));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APairHiddenMarkovModel, DecodesASequenceOfObservations) {
  std::vector<Sequences> tests = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}},
  };

  std::vector<PairHiddenMarkovModel::LabelerReturn> expected = {
    { 1.0       , {0, 1, 4}                , tests[0], {} },
    { 1.0       , {0, 1, 4}                , tests[1], {} },
    { 0.418035  , {0, 1, 1, 1, 4}          , tests[2], {} },
    { 0.0216255 , {0, 1, 1, 1, 1, 1, 1, 4} , tests[3], {} },
  };

  for (unsigned int t = 0; t < tests.size(); t++) {
    auto [ estimation, label, alignment, _ ] = phmm->posteriorDecoding(tests[t]);

    /**/ std::cerr << "prob: " << estimation << std::endl;

    EXPECT_THAT(label, Eq(expected[t].label));
    EXPECT_THAT(alignment, Eq(expected[t].alignment));
    EXPECT_THAT(DOUBLE(estimation),
        DoubleNear(DOUBLE(expected[t].estimation), 1e-7));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APairHiddenMarkovModel, ShouldDrawLabeledSequenceWithDefaultSeed) {
  std::vector<std::size_t> tests = {
    0, 1, 2, 3, 4, 5
  };

  std::vector<PairHiddenMarkovModel::GeneratorReturn<Sequence>> expected = {
    { { 0, 4                }, { {               }, {               } } },
    { { 0, 1, 4             }, { { 1             }, { 1             } } },
    { { 0, 1, 1, 4          }, { { 1, 1          }, { 0, 1          } } },
    { { 0, 3, 1, 1, 4       }, { { 1, 1, 0       }, { 2, 1, 0       } } },
    { { 0, 1, 3, 3, 1, 4    }, { { 1, 1, 0, 1    }, { 1, 2, 2, 1    } } },
    { { 0, 1, 1, 1, 3, 3, 4 }, { { 1, 0, 0, 0, 0 }, { 1, 1, 0, 2, 2 } } },
  };

  auto rng = RandomNumberGeneratorAdapter<std::mt19937>::make();

  for (unsigned int i = 0; i < tests.size(); i++) {
    auto [ label, alignment ] = phmm->drawSequence(rng, tests[i]);

    EXPECT_THAT(label, ContainerEq(expected[i].label));
    EXPECT_THAT(alignment, ContainerEq(expected[i].alignment));
  }
}

/*----------------------------------------------------------------------------*/
