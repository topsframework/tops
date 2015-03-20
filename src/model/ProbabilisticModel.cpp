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

// ToPS headers
#include "model/ProbabilisticModel.hpp"

namespace tops {
namespace model {

Sequence ProbabilisticModel::chooseSequence(Sequence &s,
                                            unsigned int size,
                                            unsigned int phase) const {
  for (unsigned int k = 0; k < size; k++) {
    if (k < s.size())
      s[k] = choosePosition(s, k);
    else
      s.push_back(choosePosition(s, k));
  }
  return s;
}

double ProbabilisticModel::evaluateSequence(const Sequence &s,
                                          unsigned int begin,
                                          unsigned int end,
                                          unsigned int phase) const {
  double prob = 0;
  for (unsigned int i = begin; i < end; i++)
    prob += evaluatePosition(s, i);
  return prob;
}

InhomogeneousMarkovChain* ProbabilisticModel::inhomogeneous() {
  return NULL;
}

EvaluatorPtr ProbabilisticModel::evaluate(const Sequence &s,
                                          bool cached) {
  if (cached)
    return std::static_pointer_cast<Evaluator>(
        CachedEvaluator<ProbabilisticModel>::make(
        shared_from_this(), s, cache(s.size() + 1)));
  return std::static_pointer_cast<Evaluator>(
      SimpleEvaluator<ProbabilisticModel>::make(shared_from_this(), s));
}

// TODO(igorbonadio): It is just a concept test.
void ProbabilisticModel::initializePrefixSumArray(
    CEPtr evaluator,
    unsigned int phase) {
  auto &prefix_sum_array = evaluator->memory();
  prefix_sum_array[0] = 0;
  for (unsigned int i = 0; i < evaluator->sequence.size() ; i++)
    prefix_sum_array[i+1] = prefix_sum_array[i]
        + evaluatePosition(evaluator->sequence, i);
}

double ProbabilisticModel::evaluateWithPrefixSumArray(
    CEPtr evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int phase) const {
  auto &prefix_sum_array = evaluator->memory();
  return prefix_sum_array[end] - prefix_sum_array[begin];
}

}  // namespace model
}  // namespace tops
