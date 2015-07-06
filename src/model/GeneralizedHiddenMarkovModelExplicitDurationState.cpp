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
#include "GeneralizedHiddenMarkovModelExplicitDurationState.hpp"
#include "LazzyRange.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

GeneralizedHiddenMarkovModelExplicitDurationState
::GeneralizedHiddenMarkovModelExplicitDurationState(
    Symbol symbol,
    ProbabilisticModelPtr observation,
    DiscreteIIDModelPtr transition,
    DiscreteIIDModelPtr duration)
      : GeneralizedHiddenMarkovModelState(symbol, observation, transition),
        _duration(duration) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

GeneralizedHiddenMarkovModelExplicitDurationStatePtr
GeneralizedHiddenMarkovModelExplicitDurationState::make(
    Symbol symbol,
    ProbabilisticModelPtr observation,
    DiscreteIIDModelPtr transition,
    DiscreteIIDModelPtr duration) {
  return GeneralizedHiddenMarkovModelExplicitDurationStatePtr(
    new GeneralizedHiddenMarkovModelExplicitDurationState(
      symbol, observation, transition, duration));
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

double GeneralizedHiddenMarkovModelExplicitDurationState::durationProbability(int l) const {
  return _duration->probabilityOf(l);
}

/*----------------------------------------------------------------------------*/

bool GeneralizedHiddenMarkovModelExplicitDurationState::isGeometricDuration() const {
  return false;
}

/*----------------------------------------------------------------------------*/

int GeneralizedHiddenMarkovModelExplicitDurationState::maximumDurationSize() const {
  return -1;
}

/*----------------------------------------------------------------------------*/

RangePtr GeneralizedHiddenMarkovModelExplicitDurationState::durations() const {
  return std::make_shared<LazzyRange>(1, _max_duration);
}

/*----------------------------------------------------------------------------*/

}
}
