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
#include "model/ExplicitDuration.hpp"

// ToPS headers
#include "model/LazzyRange.hpp"

namespace tops {
namespace model {


/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

ExplicitDuration::ExplicitDuration(ProbabilisticModelPtr duration,
                                   unsigned int max_duration_size)
    : _duration(std::move(duration)), _max_duration_size(max_duration_size) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

RangePtr ExplicitDuration::range() const {
  return std::make_shared<LazzyRange>(1, _max_duration_size);
}

/*----------------------------------------------------------------------------*/

unsigned int ExplicitDuration::maximumSize() const {
  return 0;
}

/*----------------------------------------------------------------------------*/

Probability
ExplicitDuration::probabilityOfLenght(unsigned int length) const {
  return _duration->standardEvaluator(Sequence{length})->evaluateSymbol(0);
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
