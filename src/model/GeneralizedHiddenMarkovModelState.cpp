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

// ToPS headers
#include "GeneralizedHiddenMarkovModelState.hpp"

namespace tops {
namespace model {

GeneralizedHiddenMarkovModelState::GeneralizedHiddenMarkovModelState(
    Symbol symbol,
    ProbabilisticModelPtr observation,
    DiscreteIIDModelPtr transition)
      : _symbol(symbol),
        _observation(observation),
        _transition(transition) {
}

GeneralizedHiddenMarkovModelStatePtr GeneralizedHiddenMarkovModelState::make(
    Symbol symbol,
    ProbabilisticModelPtr observation,
    DiscreteIIDModelPtr transition) {
  return GeneralizedHiddenMarkovModelStatePtr(
    new GeneralizedHiddenMarkovModelState(
      symbol, observation, transition));
}

Symbol GeneralizedHiddenMarkovModelState::symbol() {
  return _symbol;
}

ProbabilisticModelPtr GeneralizedHiddenMarkovModelState::observation(){
  return _observation;
}

DiscreteIIDModelPtr GeneralizedHiddenMarkovModelState::transition(){
  return _transition;
}

void GeneralizedHiddenMarkovModelState::addPredecessor(int id) {
  _predecessors.push_back(id);
}

void GeneralizedHiddenMarkovModelState::addSuccessor(int id) {
  _successors.push_back(id);
}

std::vector<int>& GeneralizedHiddenMarkovModelState::predecessors() {
  return _predecessors;
}

std::vector<int>& GeneralizedHiddenMarkovModelState::successors() {
  return _successors;
}

double GeneralizedHiddenMarkovModelState::durationProbability(int l) const {
  if (l == 1)
    return 0.0;
  return -HUGE;
}

bool GeneralizedHiddenMarkovModelState::isGeometricDuration() const {
  return true;
}

int GeneralizedHiddenMarkovModelState::maximumDurationSize() const {
  return 1;
}

}
}
