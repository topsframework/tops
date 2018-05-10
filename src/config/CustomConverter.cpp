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
#include "config/CustomConverter.hpp"

// Standard headers
#include <utility>

namespace tops {
namespace config {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

CustomConverter::CustomConverter(OutToInFunction out_to_in,
                                 InToOutFunction in_to_out)
    : out_to_in_(std::move(out_to_in)),
      in_to_out_(std::move(in_to_out)) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

model::Symbol CustomConverter::convert(const option::Symbol &orig) const {
  return out_to_in_(orig);
}

/*----------------------------------------------------------------------------*/

option::Symbol CustomConverter::convert(const model::Symbol &orig) const {
  return in_to_out_(orig);
}

/*----------------------------------------------------------------------------*/

}  // namespace config
}  // namespace tops
