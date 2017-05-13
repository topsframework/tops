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
using tops::model::Labeling;
using tops::model::Sequence;
using tops::model::Sequences;
using tops::model::Estimation;
using tops::model::PairHiddenMarkovModel;
using tops::model::PairHiddenMarkovModelPtr;

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

TEST_F(APairHiddenMarkovModel, FindsTheBestPath) {
  std::vector<Sequences> tests = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}}
  };

  std::vector<Estimation<Labeling<Sequences>>> expected_results = {
    { { tests[0], {0, 1, 4} }, 0.0 },
    { { tests[1], {0, 1, 4} }, 0.0 },
    { { tests[2], {0, 1, 1, 1, 4} }, 0.0 },
    { { tests[3], {0, 1, 1, 1, 1, 1, 1, 4} }, 0.0 }
  };

  for (unsigned int i = 0; i < tests.size(); i++) {
    Cube gammas;
    auto estimation = phmm->viterbi(tests[i], gammas);

    /**/ std::cerr << "prob: " << estimation.probability() << std::endl;

    EXPECT_THAT(estimation.estimated().observation(),
        Eq(expected_results[i].estimated().observation()));
    EXPECT_THAT(estimation.estimated().label(),
        Eq(expected_results[i].estimated().label()));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APairHiddenMarkovModel, DecodesASequenceOfObservations) {
  std::vector<Sequences> tests = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}}
  };

  std::vector<Estimation<Labeling<Sequences>>> expected_results = {
    { { tests[0], {0, 1, 4} }, 0.0 },
    { { tests[1], {0, 1, 4} }, 0.0 },
    { { tests[2], {0, 1, 1, 1, 4} }, 0.0 },
    { { tests[3], {0, 1, 1, 1, 1, 1, 1, 4} }, 0.0 }
  };

  for (unsigned int i = 0; i < tests.size(); i++) {
    Cube probabilities;
    auto estimation = phmm->posteriorDecoding(tests[i], probabilities);

    /**/ std::cerr << "prob: " << estimation.probability() << std::endl;

    EXPECT_THAT(estimation.estimated().observation(),
        Eq(expected_results[i].estimated().observation()));
    EXPECT_THAT(estimation.estimated().label(),
        Eq(expected_results[i].estimated().label()));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APairHiddenMarkovModel, CalculatesForwardAndBackwardProbabilities) {
  std::vector<Sequences> tests = {
    {{0}, {0}},
    {{1}, {0}},
    {{0, 0, 0}, {0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}}
  };

  for (unsigned int i = 0; i < tests.size(); i++) {
    Cube alphas, betas;
    auto prob_f = phmm->forward(tests[i], alphas);
    auto prob_b = phmm->backward(tests[i], betas);

    /**/ std::cerr << "prob_f: " << prob_f << " log: " << prob_f.data() << std::endl;
    /**/ std::cerr << "prob_b: " << prob_b << " log: " << prob_b.data() << std::endl;

    EXPECT_THAT(DOUBLE(prob_f), DoubleNear(DOUBLE(prob_b), 1e-4));
  }
}
