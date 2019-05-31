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
#include "model/GeneralizedHiddenMarkovModel.hpp"
#include "helper/GeneralizedHiddenMarkovModel.hpp"

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
using tops::model::Sequences;
using tops::model::GeneralizedHiddenMarkovModel;
using tops::model::GeneralizedHiddenMarkovModelPtr;
using tops::model::RandomNumberGeneratorAdapter;

using tops::exception::NotYetImplemented;

using tops::helper::createDishonestCoinCasinoGHMM;
using tops::helper::createUntrainedDishonestCoinCasinoGHMM;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class AGeneralizedHiddenMarkovModel : public testing::Test {
 protected:
  GeneralizedHiddenMarkovModelPtr ghmm = createDishonestCoinCasinoGHMM();
};

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(AGeneralizedHiddenMarkovModel, CalculatesForwardAndBackwardProbabilities) {
  std::vector<Sequences> tests = {
    {{0}},
    {{1}},
    {{0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}},
  };

  for (const auto& test : tests) {
    auto [ prob_f, alphas ] = ghmm->forward(test);
    auto [ prob_b, betas ] = ghmm->backward(test);

    ASSERT_THAT(DOUBLE(prob_f), DoubleNear(DOUBLE(prob_b), 1e-7));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AGeneralizedHiddenMarkovModel, FindsTheBestPath) {
  std::vector<Sequences> tests = {
    {{0}},
    {{1}},
    {{0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}},
  };

  std::vector<GeneralizedHiddenMarkovModel::LabelerReturn> expected = {
    { 0.0225     , {0, 1, 3}                , tests[0], {} },
    { 0.036      , {0, 2, 3}                , tests[1], {} },
    { 0.00275625 , {0, 1, 1, 1, 3}          , tests[2], {} },
    { 0.00198263 , {0, 2, 2, 2, 2, 2, 2, 3} , tests[3], {} },
  };

  for (size_t t = 0; t < tests.size(); t++) {
    auto [ estimation, label, alignment, _ ] = ghmm->viterbi(tests[t]);

    EXPECT_THAT(label, Eq(expected[t].label));
    EXPECT_THAT(alignment, Eq(expected[t].alignment));
    EXPECT_THAT(DOUBLE(estimation),
        DoubleNear(DOUBLE(expected[t].estimation), 1e-7));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AGeneralizedHiddenMarkovModel, DecodesASequenceOfObservations) {
  std::vector<Sequences> tests = {
    {{0}},
    {{1}},
    {{0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}},
  };

  std::vector<GeneralizedHiddenMarkovModel::LabelerReturn> expected = {
    { 0.714286 , {0, 1, 3}                , tests[0], {} },
    { 0.615385 , {0, 2, 3}                , tests[1], {} },
    { 0.604329 , {0, 1, 1, 1, 3}          , tests[2], {} },
    { 0.196082 , {0, 2, 2, 2, 2, 2, 2, 3} , tests[3], {} },
  };

  for (size_t t = 0; t < tests.size(); t++) {
    auto [ estimation, label, alignment, _ ] = ghmm->posteriorDecoding(tests[t]);

    EXPECT_THAT(label, Eq(expected[t].label));
    EXPECT_THAT(alignment, Eq(expected[t].alignment));
    EXPECT_THAT(DOUBLE(estimation),
        DoubleNear(DOUBLE(expected[t].estimation), 1e-6));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AGeneralizedHiddenMarkovModel, DISABLED_ShouldDrawLabeledSequenceWithDefaultSeed) {
  std::vector<size_t> tests = {
    0, 1, 2, 3, 4, 5
  };

  std::vector<GeneralizedHiddenMarkovModel::GeneratorReturn<Sequence>> expected = {
    { { 0, 3                }, { {               } } },
    { { 0, 1, 3             }, { { 1             } } },
    { { 0, 1, 1, 3          }, { { 1, 1          } } },
    { { 0, 2, 1, 1, 3       }, { { 1, 1, 0       } } },
    { { 0, 1, 2, 2, 2, 3    }, { { 1, 1, 1, 1    } } },
    { { 0, 1, 1, 1, 2, 2, 3 }, { { 1, 0, 0, 1, 0 } } },
  };

  auto rng = RandomNumberGeneratorAdapter<std::mt19937>::make();

  for (size_t i = 0; i < tests.size(); i++) {
    auto [ label, alignment ] = ghmm->drawSequence(rng, tests[i]);

    EXPECT_THAT(label, ContainerEq(expected[i].label));
    EXPECT_THAT(alignment, ContainerEq(expected[i].alignment));
  }

  // for (auto size : { 4, 5, 6 }) {
  //   for (size_t i = 0; i < 32; i++) {
  //     auto [ label, alignment ] = ghmm->drawSequence(rng, size);
  //     std::cerr << "    { ";
  //
  //     std::cerr << "{ { ";
  //     for (auto l : alignment[0])
  //       std::cerr << l << ", ";
  //     std::cerr << "} }, ";
  //
  //     std::cerr << "{ ";
  //     for (auto l : label)
  //       std::cerr << l << ", ";
  //     std::cerr << "}";
  //
  //     std::cerr << " },";
  //     std::cerr << std::endl;
  //   }
  //   std::cerr << std::endl;
  // }
}
/*----------------------------------------------------------------------------*/
