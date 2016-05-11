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
#include "model/GeometricDuration.hpp"

// Internal headers
#include "model/SingleValueRange.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

GeometricDuration::GeometricDuration(unsigned int id,
                                     ProbabilisticModelPtr transition)
    : _id(id), _transition(std::move(transition)) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

RangePtr GeometricDuration::range() const {
  return std::make_shared<SingleValueRange>(1);
}

/*----------------------------------------------------------------------------*/

unsigned int GeometricDuration::maximumSize() const {
  return 1;
}

/*----------------------------------------------------------------------------*/

Probability GeometricDuration::probabilityOfLenght(unsigned int length) const {
  if (length == 1) return 0.0;
  return std::pow(
      _transition->standardEvaluator(Sequence{_id})->evaluateSymbol(0),
      length-1);
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
