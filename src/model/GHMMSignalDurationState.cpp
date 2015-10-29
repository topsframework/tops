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

// ToPS headers
#include "model/GHMMSignalDurationState.hpp"
#include "model/SingleValueRange.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

GHMMSignalDurationState::GHMMSignalDurationState(
    Id id,
    ProbabilisticModelPtr observation,
    DiscreteIIDModelPtr transition,
    unsigned int duration_size)
    : Base(std::move(id), std::move(observation), std::move(transition)),
      _duration_size(duration_size) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

Probability
GHMMSignalDurationState::durationProbability(unsigned int length) const {
  if (length == _duration_size)
    return 0.0;
  return -std::numeric_limits<double>::infinity();
}

/*----------------------------------------------------------------------------*/

unsigned int GHMMSignalDurationState::maximumDurationSize() const {
  return _duration_size;
}

/*----------------------------------------------------------------------------*/

RangePtr GHMMSignalDurationState::durations() const {
  return std::make_shared<SingleValueRange>(_duration_size);
}

/*----------------------------------------------------------------------------*/

}
}
