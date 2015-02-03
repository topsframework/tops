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
#include "PhasedInhomogeneousMarkovChain.hpp"

namespace tops {
namespace model {

PhasedInhomogeneousMarkovChainPtr PhasedInhomogeneousMarkovChain::make(
    std::vector<VariableLengthMarkovChainPtr> vlmcs) {
  return PhasedInhomogeneousMarkovChainPtr(
    new PhasedInhomogeneousMarkovChain(vlmcs));
}

PhasedInhomogeneousMarkovChain::PhasedInhomogeneousMarkovChain(
    std::vector<VariableLengthMarkovChainPtr> vlmcs)
    : InhomogeneousMarkovChain(vlmcs) {
}

double PhasedInhomogeneousMarkovChain::evaluatePosition(const Sequence &s,
                                                        unsigned int i) const {
  return _vlmcs[i % _vlmcs.size()]->evaluatePosition(s, i);
}

Symbol PhasedInhomogeneousMarkovChain::choosePosition(const Sequence &s,
                                                   unsigned int i) const {
  return _vlmcs[i % _vlmcs.size()]->choosePosition(s, i);
}

}  // namespace model
}  // namespace tops
