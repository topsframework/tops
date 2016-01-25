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
#include <limits>

// ToPS headers
#include "model/SingleValueRange.hpp"

// Interface header
#include "model/SignalDuration.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

SignalDuration::SignalDuration(unsigned int duration_size)
    : _duration_size(duration_size) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

RangePtr SignalDuration::range() const {
  return std::make_shared<SingleValueRange>(_duration_size);
}

/*----------------------------------------------------------------------------*/

unsigned int SignalDuration::maximumSize() const {
  return _duration_size;
}

/*----------------------------------------------------------------------------*/

Probability SignalDuration::probabilityOfLenght(unsigned int length) const {
  if (length == _duration_size)
    return 0.0;
  return -std::numeric_limits<Probability>::infinity();
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
