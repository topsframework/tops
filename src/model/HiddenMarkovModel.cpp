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
#include "HiddenMarkovModel.hpp"

namespace tops {
namespace model {

HiddenMarkovModelPtr HiddenMarkovModel::make(
      std::vector<HiddenMarkovModelStatePtr> states,
      DiscreteIIDModelPtr initial_probability,
      unsigned int state_alphabet_size,
      unsigned int observation_alphabet_size) {
  return HiddenMarkovModelPtr(new HiddenMarkovModel(
    states,
    initial_probability,
    state_alphabet_size,
    observation_alphabet_size));
}

HiddenMarkovModel::HiddenMarkovModel(
      std::vector<HiddenMarkovModelStatePtr> states,
      DiscreteIIDModelPtr initial_probability,
      unsigned int state_alphabet_size,
      unsigned int observation_alphabet_size)
        : _states(states),
          _initial_probability(initial_probability),
          _state_alphabet_size(state_alphabet_size),
          _observation_alphabet_size(observation_alphabet_size) {

}

double HiddenMarkovModel::evaluatePosition(const Sequence &s, unsigned int i) const {

}

Symbol HiddenMarkovModel::choosePosition(const Sequence &s, unsigned int i) const {

}

double HiddenMarkovModel::evaluateSequencesPosition(const Sequence &xs, const Sequence &ys, unsigned int i) const {
  double transition;
  if (i == 0)
    transition = _states[ys[0]]->emissions()->probabilityOf(xs[0]);
  else
    transition = _states[ys[i-1]]->transitions()->probabilityOf(ys[i]);
  return transition + _states[ys[i]]->emissions()->probabilityOf(xs[i]);
}

double HiddenMarkovModel::evaluateSequences(const Sequence &xs, const Sequence &ys, unsigned int begin, unsigned int end) const {
  double prob = 0;
  for (unsigned int i = begin; i < end; i++)
    prob += evaluateSequencesPosition(xs, ys, i);
  return prob;
}


}  // namespace model
}  // namespace tops
