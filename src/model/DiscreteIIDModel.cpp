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
#include <vector>
#include <string>
#include <cmath>

// ToPS headers
#include "model/DiscreteIIDModel.hpp"
#include "model/Random.hpp"

namespace tops {
namespace model {

DiscreteIIDModel::DiscreteIIDModel(std::vector<double> probabilities)
    : _probabilities(probabilities) {
}

DiscreteIIDModelPtr DiscreteIIDModel::make(std::vector<double> probabilities) {
  return DiscreteIIDModelPtr(new DiscreteIIDModel(probabilities));
}

DiscreteIIDModelPtr DiscreteIIDModel::trainML(
    std::vector<Sequence> training_set,
    unsigned int alphabet_size) {
  std::vector<double> probabilities(alphabet_size, 0);
  unsigned int number_of_symbols = 0;
  for (auto sequence : training_set) {
    for (auto symbol : sequence) {
      probabilities[symbol]++;
      number_of_symbols++;
    }
  }
  for (Symbol s = 0; s < alphabet_size; s++)
    probabilities[s] = log(probabilities[s]/number_of_symbols);
  return DiscreteIIDModel::make(probabilities);
}

int DiscreteIIDModel::alphabetSize() const {
  return _probabilities.size();
}

double DiscreteIIDModel::probabilityOf(Symbol s) const {
  return _probabilities[s];
}

double DiscreteIIDModel::evaluatePosition(const Sequence &s,
                                          unsigned int i) const {
  return _probabilities[s[i]];
}

Symbol DiscreteIIDModel::choosePosition(const Sequence &s,
                                        unsigned int i) const {
  double random = generateRandomDouble();
  for (unsigned int symbol = 0; symbol < _probabilities.size(); symbol++) {
    random -= exp(_probabilities[symbol]);
    if (random <= 0)
      return symbol;
  }
  return _probabilities.size()-1;
}


}  // namespace model
}  // namespace tops
