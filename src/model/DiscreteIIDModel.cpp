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

#include <vector>
#include <string>

#include "DiscreteIIDModel.hpp"

namespace tops {
namespace model {

DiscreteIIDModel::DiscreteIIDModel(std::vector<double> probabilities) : tops::DiscreteIIDModel(probabilities) {
  AlphabetPtr alphabet = AlphabetPtr(new Alphabet());
  for (auto p : probabilities)
    alphabet->createSymbol(std::to_string(p));
  setAlphabet(alphabet);
}

DiscreteIIDModelPtr DiscreteIIDModel::make(std::vector<double> probabilities) {
  return DiscreteIIDModelPtr(new DiscreteIIDModel(probabilities));
}

double DiscreteIIDModel::probabilityOf(Symbol s) const {
  return log_probability_of(s);
}

double DiscreteIIDModel::evaluatePosition(const Sequence &s, unsigned int i) const {
  return tops::DiscreteIIDModel::evaluatePosition(s, i);
}

int DiscreteIIDModel::choosePosition(const Sequence &s, unsigned int i) const {
  return static_cast<int>(tops::DiscreteIIDModel::choosePosition(s, i));
}

double DiscreteIIDModel::evaluateSequence(const Sequence &s, unsigned int begin, unsigned int end) const {
  return evaluate(s, begin, end);
}


}  // namespace model
}  // namespace tops
