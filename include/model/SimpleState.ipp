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
#include <utility>

// Internal headers
#include "model/SignalDuration.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename E, typename T, std::size_t d, bool... gaps>
SimpleState<E, T, d, gaps...>::SimpleState(Id id, EmissionModelPtr emission,
                                                  TransitionModelPtr transition,
                                                  DurationPtr duration)
    : Base(id, emission, transition, duration), _gaps{ gaps... } {
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, std::size_t d, bool... gaps>
SimpleState<E, T, d, gaps...>::SimpleState(Id id, EmissionModelPtr emission,
                                                  TransitionModelPtr transition)
    : Base(id, emission, transition, SignalDuration::make(1)),
      _gaps{ gaps... } {
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, std::size_t d, bool... gaps>
bool SimpleState<E, T, d, gaps...>::isSilent() const {
  return (... && gaps);
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, std::size_t d, bool... gaps>
bool SimpleState<E, T, d, gaps...>::hasGap(Dimension dim) const {
  return _gaps.at(dim);
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, std::size_t d, bool... gaps>
auto SimpleState<E, T, d, gaps...>::delta(Dimension dim) const
    -> Position {
  return hasGap(dim) ? 0 : 1;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
