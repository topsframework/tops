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

// ToPS headers
#include "FactorableModel.hpp"
#include "Util.hpp"

namespace tops {
namespace model {

double FactorableModel::evaluateSequence(const Sequence &s,
                                          unsigned int begin,
                                          unsigned int end) const {
  double prob = 0;
  for (unsigned int i = begin; i < end; i++)
    prob += evaluatePosition(s, i);
  return prob;
}

double FactorableModel::evaluateWithPrefixSumArray(int begin, int end) {
  return _prefix_sum_array[end] - _prefix_sum_array[begin];
}

void FactorableModel::initializePrefixSumArray(const Sequence &s) {
  _prefix_sum_array.resize(s.size() + 1);
  _prefix_sum_array[0] = 0;
  for (unsigned int i = 0; i < s.size() ; i++) {
    _prefix_sum_array[i+1] = _prefix_sum_array[i] + evaluatePosition(s, i);
  }
}

}  // namespace model
}  // namespace tops
