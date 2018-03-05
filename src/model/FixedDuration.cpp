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
#include "model/Range.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

FixedDuration::FixedDuration(std::size_t fixed_length)
    : _fixed_length(fixed_length) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

Range FixedDuration::possibleLengths(std::size_t max_length) const {
  if (_fixed_length > max_length) return { 0, 0 };
  return { _fixed_length, _fixed_length + 1 };
}

/*----------------------------------------------------------------------------*/

Probability FixedDuration::probabilityOfLenght(std::size_t length) const {
  return length == _fixed_length ? 1 : 0;
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

std::size_t FixedDuration::fixedLength() {
  return _fixed_length;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
