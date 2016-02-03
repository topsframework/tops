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
#include "model/InhomogeneousMarkovChain.hpp"

// Standard headers
#include <cmath>
#include <limits>
#include <vector>

// Internal headers
#include "exception/OutOfRange.hpp"

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
    return 0;
}

/*----------------------------------------------------------------------------*/

Standard<Symbol>
InhomogeneousMarkovChain::drawSymbol(SGPtr<Standard> generator,
                                     unsigned int pos,
                                     unsigned int phase,
                                     const Sequence& context) const {
  if (pos + phase < _vlmcs.size()) {
    auto vlmc = _vlmcs[pos + phase];
    return vlmc->standardGenerator(generator->randomNumberGenerator())
               ->drawSymbol(pos, phase, context);
  }

  throw tops::exception::OutOfRange();
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
