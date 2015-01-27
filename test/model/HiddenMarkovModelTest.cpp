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

#include <math.h>
#include <vector>

#include "gmock/gmock.h"

#include "HiddenMarkovModel.hpp"
#include "DiscreteIIDModel.hpp"
#include "Sequence.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::ContainerEq;

using tops::model::HiddenMarkovModel;
using tops::model::HiddenMarkovModelPtr;
using tops::model::Sequence;

class AHiddenMarkovModel : public testing::Test {
 protected:
  HiddenMarkovModelPtr hmm;

  virtual void SetUp() {
    auto initial_probabilities = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(std::vector<double>{0.5, 0.5}));
    auto loaded_distribution = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(std::vector<double>{0.2, 0.8}));

    auto state_alphabet = tops::AlphabetPtr(new tops::Alphabet());
    state_alphabet->createSymbol("fair");
    state_alphabet->createSymbol("loaded");

    std::vector<tops::HMMStatePtr> states;

    auto fair_state = tops::HMMStatePtr(new tops::HMMState());
    fair_state->setEmissions(initial_probabilities);
    fair_state->setTransition(initial_probabilities);
    fair_state->setName(state_alphabet->getSymbol("fair"));
    states.push_back(fair_state);

    auto loaded_state = tops::HMMStatePtr(new tops::HMMState());
    loaded_state->setEmissions(loaded_distribution);
    loaded_state->setTransition(loaded_distribution);
    loaded_state->setName(state_alphabet->getSymbol("loaded"));
    states.push_back(loaded_state);

    hmm = HiddenMarkovModel::make(states, initial_probabilities, 2, 2);
  }
};

TEST_F(AHiddenMarkovModel, ShouldFindTheBestPath) {
  Sequence path;
  tops::Matrix matrix;
  ASSERT_THAT(hmm->viterbi({0, 0}, path, matrix), DoubleEq(log(0.0625)));
  ASSERT_THAT(path, ContainerEq(Sequence{0, 0}));
  ASSERT_THAT(hmm->viterbi({0, 0, 0, 0, 0, 0, 0}, path, matrix), DoubleNear(log(0.000061035188), 0.000001));
  ASSERT_THAT(path, ContainerEq(Sequence{0, 0, 0, 0, 0, 0, 0}));
  ASSERT_THAT(hmm->viterbi({1, 1, 1, 1, 1, 1}, path, matrix), DoubleNear(log(0.0429), 0.01));
  ASSERT_THAT(path, ContainerEq(Sequence{1, 1, 1, 1, 1, 1}));
}
