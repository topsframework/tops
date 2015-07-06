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
#include "model/InhomogeneousMarkovChain.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

InhomogeneousMarkovChain::InhomogeneousMarkovChain(
    std::vector<VariableLengthMarkovChainPtr> vlmcs)
    : _vlmcs(vlmcs) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

InhomogeneousMarkovChainPtr InhomogeneousMarkovChain::make(
    std::vector<VariableLengthMarkovChainPtr> vlmcs) {
  return InhomogeneousMarkovChainPtr(
    new InhomogeneousMarkovChain(vlmcs));
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

Probability
InhomogeneousMarkovChain::evaluateSymbol(SEPtr<Standard> evaluator,
                                         unsigned int pos,
                                         unsigned int phase) const {
  if (pos + phase < _vlmcs.size())
    return _vlmcs[pos + phase]->standardEvaluator(
             evaluator->sequence())->evaluateSymbol(pos);
  else
    return -std::numeric_limits<double>::infinity();
}

/*----------------------------------------------------------------------------*/

Standard<Symbol>
InhomogeneousMarkovChain::drawSymbol(SGPtr<Standard> /* generator */,
                                     unsigned int pos,
                                     unsigned int phase,
                                     const Sequence &context) const {
  if (pos + phase < _vlmcs.size()) {
    auto vlmc = _vlmcs[pos + phase];
    return vlmc->standardGenerator()->drawSymbol(pos, phase, context);
  }

  // TODO(igorbonadio): Throw exception!
  return Standard<Symbol>(INVALID_SYMBOL);
}

/*----------------------------------------------------------------------------*/
/*                             VIRTUAL METHODS                                */
/*----------------------------------------------------------------------------*/

unsigned int InhomogeneousMarkovChain::maximumTimeValue() {
  return _vlmcs.size();
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
