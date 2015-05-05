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

PhasedInhomogeneousMarkovChainPtr
  PhasedInhomogeneousMarkovChain::trainInterpolatedPhasedMarkovChain(
    std::vector<Sequence> training_set,
    unsigned int alphabet_size,
    unsigned int order,
    unsigned int nphases,
    double pseudo_counts,
    std::vector<double> weights,
    ProbabilisticModelPtr apriori) {

  unsigned int length = nphases;
  std::vector<VariableLengthMarkovChainPtr> vlmcs(length);

  for (unsigned int i = 0; i < length; i++) {
    std::vector<double> positional_weights;
    std::vector<Sequence> positionalSample;
    for (unsigned int j = 0; j < training_set.size(); j++) {
      int nseq = 0;
      while (true) {
        int start = length * nseq - order + i;
        if (start < 0) {
          nseq++;
          continue;
        }
        unsigned int end = length * nseq + i;
        if (end >= training_set[j].size())
          break;
        Sequence s;
        for (unsigned int k = start; k <= end; k++) {
          s.push_back(training_set[j][k]);
          positional_weights.push_back(weights[j]);
        }
        positionalSample.push_back(s);
        nseq++;
      }
    }

    ContextTreePtr tree = ContextTree::make(alphabet_size);

    if (apriori != NULL) {
      tree->initializeCounter(positionalSample, order, 0, positional_weights);
      tree->pruneTreeSmallSampleSize(400);
      tree->normalize(apriori, pseudo_counts);
    } else {
      tree->initializeCounter(positionalSample, order, pseudo_counts,
                              positional_weights);
      tree->pruneTreeSmallSampleSize(400);
      tree->normalize();
    }

    vlmcs[i] = VariableLengthMarkovChain::make(tree);
  }

  return PhasedInhomogeneousMarkovChain::make(vlmcs);
}

double PhasedInhomogeneousMarkovChain::evaluate(
    const Sequence &s,
    unsigned int pos,
    unsigned int phase) const {
  return _vlmcs[(pos + phase) % _vlmcs.size()]->evaluate(s, pos);
}

Symbol PhasedInhomogeneousMarkovChain::choose(
    const Sequence &s,
    unsigned int pos,
    unsigned int phase) const {
  return _vlmcs[(pos + phase) % _vlmcs.size()]->choose(s, pos);
}

EvaluatorPtr PhasedInhomogeneousMarkovChain::evaluator(const Sequence &s,
                                                      bool cached) {
  if (cached)
    return Evaluator::make(
      CachedEvaluatorImpl<PhasedInhomogeneousMarkovChain>::make(
        std::static_pointer_cast<PhasedInhomogeneousMarkovChain>(shared_from_this()),
        s, Cache(_vlmcs.size(), std::vector<double>(s.size() + 1))));
  return Evaluator::make(
    SimpleEvaluatorImpl<PhasedInhomogeneousMarkovChain>::make(
      std::static_pointer_cast<PhasedInhomogeneousMarkovChain>(shared_from_this()),
      s));
}

double PhasedInhomogeneousMarkovChain::simpleProbabilityOf(
    SEPtr evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int phase) const {
  double prob = 0;
  for (unsigned int i = begin; i < end; i++)
    prob += evaluate(evaluator->sequence(), i);
  return prob;
}

void PhasedInhomogeneousMarkovChain::initializeCachedEvaluator(
    CEPtr evaluator,
    unsigned int phase) {
  auto &prefix_sum_matrix = evaluator->cache();
  for (unsigned int t = 0; t < _vlmcs.size() ; t++) {
    prefix_sum_matrix[t][0] = 0;
    for (unsigned int i = 0; i < evaluator->sequence().size() ; i++) {
      prefix_sum_matrix[t][i+1] = prefix_sum_matrix[t][i] + evaluate(evaluator->sequence(), i, t);
    }
  }
}

double PhasedInhomogeneousMarkovChain::cachedProbabilityOf(
    CEPtr evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int phase) const {
  auto &prefix_sum_matrix = evaluator->cache();
  return prefix_sum_matrix[phase][end] - prefix_sum_matrix[phase][begin];
}


}  // namespace model
}  // namespace tops
