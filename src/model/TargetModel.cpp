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
#include <cmath>
#include <vector>

// ToPS headers
#include "model/TargetModel.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

TargetModel::TargetModel(int alphabet_size)
    : DiscreteIIDModel(
      std::vector<double>(alphabet_size, log(1.0/alphabet_size))) {
}

/*----------------------------------------------------------------------------*/
/*                              STATIC METHODS                                */
/*----------------------------------------------------------------------------*/

TargetModelPtr TargetModel::make(int alphabet_size) {
  return TargetModelPtr(new TargetModel(alphabet_size));
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

Probability TargetModel::evaluateSymbol(SEPtr<Standard> evaluator,
                                        unsigned int pos,
                                        unsigned int phase) const {
  return sequenceDistribution(evaluator->sequence())
    ->standardEvaluator(evaluator->sequence())->evaluateSymbol(pos);
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

DiscreteIIDModelPtr TargetModel::sequenceDistribution(const Sequence &s) const {
  return DiscreteIIDModel::trainML({s}, alphabetSize());
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
