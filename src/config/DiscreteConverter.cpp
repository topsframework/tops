/***********************************************************************/
/*  Copyright 2016 ToPS                                                */
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
#include "config/DiscreteConverter.hpp"

// Standard headers
#include <utility>

namespace tops {
namespace config {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

DiscreteConverter::DiscreteConverter(option::Alphabet alphabet)
    : alphabet_(alphabet) {
  model::Symbol i = 0;

  for (const option::Symbol &s : alphabet) {
    out_to_in_[s] = i;
    in_to_out_[i] = s;
    ++i;
  }
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

model::Symbol DiscreteConverter::convert(const option::Symbol &orig) const {
  return out_to_in_.at(orig);
}

/*----------------------------------------------------------------------------*/

option::Symbol DiscreteConverter::convert(const model::Symbol &orig) const {
  return in_to_out_.at(orig);
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

option::Alphabet& DiscreteConverter::alphabet() {
  return alphabet_;
}

/*----------------------------------------------------------------------------*/

const option::Alphabet& DiscreteConverter::alphabet() const {
  return alphabet_;
}

/*----------------------------------------------------------------------------*/

}  // namespace config
}  // namespace tops
