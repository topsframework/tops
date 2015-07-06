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
#include "GeneralizedHiddenMarkovModelSignalState.hpp"
#include "SingleValueRange.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

GeneralizedHiddenMarkovModelSignalState::GeneralizedHiddenMarkovModelSignalState(
    Symbol symbol,
    ProbabilisticModelPtr observation,
    DiscreteIIDModelPtr transition,
    int duration_size)
      : GeneralizedHiddenMarkovModelState(symbol, observation, transition),
        _duration_size(duration_size) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

GeneralizedHiddenMarkovModelSignalStatePtr
GeneralizedHiddenMarkovModelSignalState::make(
    Symbol symbol,
    ProbabilisticModelPtr observation,
    DiscreteIIDModelPtr transition,
    int duration_size) {
  return GeneralizedHiddenMarkovModelSignalStatePtr(
    new GeneralizedHiddenMarkovModelSignalState(
      symbol, observation, transition, duration_size));
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

double GeneralizedHiddenMarkovModelSignalState::durationProbability(int l) const {
  if (l == _duration_size)
    return 0.0;
  return -std::numeric_limits<double>::infinity();
}

/*----------------------------------------------------------------------------*/

int GeneralizedHiddenMarkovModelSignalState::maximumDurationSize() const {
  return _duration_size;
}

/*----------------------------------------------------------------------------*/

RangePtr GeneralizedHiddenMarkovModelSignalState::durations() const {
  return std::make_shared<SingleValueRange>(_duration_size);
}

/*----------------------------------------------------------------------------*/

}
}
