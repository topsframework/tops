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

// Standard headers
#include <memory>
#include <utility>

// Internal headers
#include "model/Range.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

ExplicitDuration::ExplicitDuration(ProbabilisticModelPtr duration)
    : _duration_model(std::move(duration)) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

Range ExplicitDuration::possibleLengths(size_t max_length) const {
  return { 1, max_length + 1 };
}

/*----------------------------------------------------------------------------*/

Probability ExplicitDuration::probabilityOfLenght(size_t len) const {
  auto evaluator = _duration_model->standardEvaluator({ Sequence{len} });
  return evaluator->evaluateSymbol(0);
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

ProbabilisticModelPtr ExplicitDuration::durationModel() {
  return _duration_model;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
