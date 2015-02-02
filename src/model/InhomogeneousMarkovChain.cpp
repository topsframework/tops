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

#include "InhomogeneousMarkovChain.hpp"

namespace tops {
namespace model {

InhomogeneousMarkovChainPtr InhomogeneousMarkovChain::make(std::vector<VariableLengthMarkovChainPtr> vlmcs, bool phased) {
  return InhomogeneousMarkovChainPtr(new InhomogeneousMarkovChain(vlmcs, phased));
}

InhomogeneousMarkovChain::InhomogeneousMarkovChain(std::vector<VariableLengthMarkovChainPtr> vlmcs, bool phased) : _vlmcs(vlmcs), _phased(phased) {
}

int InhomogeneousMarkovChain::alphabetSize() const {
  return _vlmcs[0]->alphabetSize();
}

double InhomogeneousMarkovChain::evaluatePosition(const Sequence &s, unsigned int i) const {
  if (_phased)
    return _vlmcs[i % _vlmcs.size()]->evaluatePosition(s, i);
  else if (i < _vlmcs.size())
    return _vlmcs[i]->evaluatePosition(s, i);
  else
    return -HUGE;
}

int InhomogeneousMarkovChain::choosePosition(const Sequence &s, unsigned int i) const {
  if (_phased)
    return _vlmcs[i % _vlmcs.size()]->choosePosition(s, i);
  else if (i < _vlmcs.size())
    return _vlmcs[i]->choosePosition(s, i);
  else
    return 0; // TODO: ERROR!
}

double InhomogeneousMarkovChain::evaluateSequence(const Sequence &s, unsigned int begin, unsigned int end) const {
  double p = 0;
  for (int i = 0; i < s.size(); i++) {
    p += evaluatePosition(s, i);
  }
  return p;
}

}
}
