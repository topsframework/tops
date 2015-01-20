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

#include "DiscreteIIDModel.hpp"

#include <string>
#include <vector>

#include "src/DiscreteIIDModel.hpp"

namespace tops {
namespace model {

DiscreteIIDModel::DiscreteIIDModel(const std::vector<double> &probabilities) {
  auto model = tops::DiscreteIIDModelPtr(
    new tops::DiscreteIIDModel(probabilities));
  auto alphabet = tops::AlphabetPtr(new Alphabet());
  for (auto p : probabilities) {
    alphabet->createSymbol(std::to_string(p));
  }
  model->setAlphabet(alphabet);
  _self = model;
}

int DiscreteIIDModel::alphabetSize() {
  return _self->alphabet()->size();
}

double DiscreteIIDModel::choose() const {
  return _self->choose();
}

Symbol DiscreteIIDModel::choosePosition(const Sequence &sequence,
                                        unsigned int index) const {
  return _self->factorable()->choosePosition(sequence, index);
}

double DiscreteIIDModel::logProbabilityOf(Symbol symbol) {
  return _self->log_probability_of(symbol);
}

double DiscreteIIDModel::evaluate(const Sequence &sequence,
                                  unsigned int begin,
                                  unsigned int end) {
  return _self->evaluate(sequence, begin, end);
}

double DiscreteIIDModel::evaluatePosition(const Sequence &sequence,
                                          unsigned int index) const {
  return _self->evaluatePosition(sequence, index);
}

}  // namespace model
}  // namespace tops
