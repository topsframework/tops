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

// Interface header
#include "model/FixedDuration.hpp"

// Standard headers
#include <limits>
#include <memory>

// Internal headers
#include "model/SingleValueRange.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

FixedDuration::FixedDuration(unsigned int duration_size)
    : _duration_size(duration_size) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

RangePtr FixedDuration::range() const {
  return std::make_shared<SingleValueRange>(_duration_size);
}

/*----------------------------------------------------------------------------*/

unsigned int FixedDuration::maximumSize() const {
  return _duration_size;
}

/*----------------------------------------------------------------------------*/

Probability FixedDuration::probabilityOfLenght(unsigned int length) const {
  if (length == _duration_size) return 1;
  return 0;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
