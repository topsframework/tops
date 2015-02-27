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
#include "model/HiddenMarkovModel.hpp"
#include "model/Sequence.hpp"
#include "model/Util.hpp"

#include "helper/HiddenMarkovModel.hpp"
#include "helper/Sequence.hpp"

using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::Eq;

using tops::model::HiddenMarkovModel;
using tops::model::HiddenMarkovModelPtr;
using tops::model::Sequence;
using tops::model::Matrix;
using tops::model::log_sum;

using tops::helper::createDishonestCoinCasinoHMM;
using tops::helper::generateAllCombinationsOfSymbols;

class AHiddenMarkovModel : public testing::Test {
 protected:
  HiddenMarkovModelPtr hmm = createDishonestCoinCasinoHMM();
};

TEST_F(AHiddenMarkovModel, ShouldHaveEvaluateTheJointProbability) {
  ASSERT_THAT(hmm->evaluateSequences({0, 0, 1}, {0, 1, 1}, 0, 3),
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
    Sequence path;
    Matrix gamma;
    ASSERT_THAT(hmm->viterbi(test[0], path, gamma), DoubleEq(hmm->evaluateSequences(test[0], test[1], 0, test[0].size())));
    ASSERT_THAT(path, Eq(test[1]));
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
    std::vector<Sequence> labels = generateAllCombinationsOfSymbols(observations.size());
    for (auto y : labels) {
      px = log_sum(px, hmm->evaluateSequences(observations, y, 0, observations.size()));
    }
    Matrix alpha;
    ASSERT_THAT(hmm->forward(observations, alpha), DoubleEq(px));
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
    Sequence path;
    Matrix gamma;
    hmm->posteriorDecoding(test[0], path, gamma);
    ASSERT_THAT(path, Eq(test[1]));
  }
}