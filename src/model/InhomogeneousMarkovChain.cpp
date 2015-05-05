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
#include "InhomogeneousMarkovChain.hpp"

namespace tops {
namespace model {

InhomogeneousMarkovChainPtr InhomogeneousMarkovChain::make(
    std::vector<VariableLengthMarkovChainPtr> vlmcs) {
  return InhomogeneousMarkovChainPtr(
    new InhomogeneousMarkovChain(vlmcs));
}

InhomogeneousMarkovChain::InhomogeneousMarkovChain(
    std::vector<VariableLengthMarkovChainPtr> vlmcs)
    : _vlmcs(vlmcs) {
}

double InhomogeneousMarkovChain::evaluate(const Sequence &s,
                                          unsigned int pos,
                                          unsigned int phase) const {
  if (pos + phase < _vlmcs.size())
    return _vlmcs[pos + phase]->evaluate(s, pos);
  else
    return -HUGE;
}

Symbol InhomogeneousMarkovChain::choose(const Sequence &s,
                                        unsigned int pos,
                                        unsigned int phase) const {
  if (pos + phase < _vlmcs.size())
    return _vlmcs[pos + phase]->choose(s, pos);
  else
    return 0;  // TODO(igorbonadio): ERROR!
}

InhomogeneousMarkovChain* InhomogeneousMarkovChain::inhomogeneous() {
  return this;
}

unsigned int InhomogeneousMarkovChain::maximumTimeValue() {
  return _vlmcs.size();
}

}  // namespace model
}  // namespace tops
