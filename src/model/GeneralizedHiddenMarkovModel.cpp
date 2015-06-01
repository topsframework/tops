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
#include <algorithm>

// ToPS headers
#include "model/GeneralizedHiddenMarkovModel.hpp"
#include "model/Util.hpp"
#include "model/Segment.hpp"

namespace tops {
namespace model {

GeneralizedHiddenMarkovModelPtr GeneralizedHiddenMarkovModel::make(
    std::vector<GeneralizedHiddenMarkovModelStatePtr> states,
    DiscreteIIDModelPtr initial_probability,
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size) {
  return GeneralizedHiddenMarkovModelPtr(new GeneralizedHiddenMarkovModel(
    states,
    initial_probability,
    state_alphabet_size,
    observation_alphabet_size));
}

GeneralizedHiddenMarkovModel::GeneralizedHiddenMarkovModel(
    std::vector<GeneralizedHiddenMarkovModelStatePtr> states,
    DiscreteIIDModelPtr initial_probabilities,
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size)
    : _states(states),
      _initial_probabilities(initial_probabilities),
      _state_alphabet_size(state_alphabet_size),
      _observation_alphabet_size(observation_alphabet_size) {
}

double GeneralizedHiddenMarkovModel::evaluate(
    const Sequence &xs,
    const Sequence &ys) const {
  double prob = 0;
  auto segments = Segment::readSequence(ys);
  for (unsigned int i = 0; i < segments.size(); i++) {
    if (i == 0) {
      prob += _initial_probabilities->probabilityOf(segments[i].symbol());
    } else {
      prob += _states[segments[i-1].symbol()]->transition()->probabilityOf(segments[i].symbol());
    }
    prob += _states[segments[i].symbol()]->durationProbability(segments[i].end() - segments[i].begin());
    prob += _states[segments[i].symbol()]->observation()->evaluator(xs)->probabilityOf(segments[i].begin(), segments[i].end());
  }
  return prob;
}

}  // namespace model
}  // namespace tops
