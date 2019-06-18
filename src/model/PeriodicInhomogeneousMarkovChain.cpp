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
#include "model/PeriodicInhomogeneousMarkovChain.hpp"

// Standard headers
#include <cmath>
#include <limits>
#include <vector>

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

PeriodicInhomogeneousMarkovChain::PeriodicInhomogeneousMarkovChain(
    std::vector<VariableLengthMarkovChainPtr> vlmcs)
    : _vlmcs(vlmcs) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

/*================================  TRAINER  =================================*/

PeriodicInhomogeneousMarkovChainPtr
PeriodicInhomogeneousMarkovChain::train(TrainerPtr<Multiple, Self> trainer,
                                        interpolation_algorithm,
                                        size_t alphabet_size,
                                        size_t order,
                                        size_t nphases,
                                        double pseudo_counts,
                                        std::vector<double> weights,
                                        ProbabilisticModelPtr apriori) {
  auto& training_set = trainer->training_set();

  size_t length = nphases;
  std::vector<VariableLengthMarkovChainPtr> vlmcs(length);

  for (size_t i = 0; i < length; i++) {
    std::vector<double> positional_weights;
    std::vector<Multiple<Sequence>> positionalSample;
    for (size_t j = 0; j < training_set.size(); j++) {
      int nseq = 0;
      while (true) {
        int start = length * nseq - order + i;
        if (start < 0) {
          nseq++;
          continue;
        }
        size_t end = length * nseq + i;
        if (end >= training_set[j][0].size())
          break;
        Sequence s;
        for (size_t k = start; k <= end; k++) {
          s.push_back(training_set[j][0][k]);
          positional_weights.push_back(weights[j]);
        }
        positionalSample.push_back({ s });
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

  return PeriodicInhomogeneousMarkovChain::make(vlmcs);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

Probability
PeriodicInhomogeneousMarkovChain::evaluateSymbol(SEPtr<Multiple> evaluator,
                                                 size_t pos,
                                                 size_t phase) const {
  return _vlmcs[phase]->standardEvaluator(evaluator->sequence())
                      ->evaluateSymbol(pos);
}

/*----------------------------------------------------------------------------*/

Probability
PeriodicInhomogeneousMarkovChain::evaluateSequence(SEPtr<Multiple> evaluator,
                                                   size_t begin,
                                                   size_t end,
                                                   size_t phase) const {
  auto t = phase;
  Probability prob = 1;
  for (size_t i = begin; i < end; i++) {
    prob *= evaluator->evaluateSymbol(i, t);
    t = (t + 1) % (_vlmcs.size());
  }
  return prob;
}

/*----------------------------------------------------------------------------*/

void PeriodicInhomogeneousMarkovChain::initializeCache(
    CEPtr<Multiple> evaluator, size_t /* phase */) {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(_vlmcs.size());
  for (auto k = 0u; k < _vlmcs.size(); k++) {
    prefix_sum_array[k].resize(evaluator->sequence()[0].size() + 1);
    prefix_sum_array[k][0] = 1.0;
    for (auto i = 0u; i < evaluator->sequence()[0].size(); i++) {
      auto phase = (i + k) % _vlmcs.size();
      prefix_sum_array[k][i + 1] = prefix_sum_array[k][i] * evaluator->evaluateSymbol(i, phase);
    }
  }
}

/*----------------------------------------------------------------------------*/

Probability
PeriodicInhomogeneousMarkovChain::evaluateSymbol(CEPtr<Multiple> evaluator,
                                                 size_t pos,
                                                 size_t phase) const {
  return Base::evaluateSymbol(evaluator, pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability
PeriodicInhomogeneousMarkovChain::evaluateSequence(CEPtr<Multiple> evaluator,
                                                   size_t begin,
                                                   size_t end,
                                                   size_t phase) const {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;

  auto p = begin % _vlmcs.size();
  auto k = 0u;
  while (((p + k) % _vlmcs.size()) != phase)
    k++;
  p = k;

  return prefix_sum_array[p][end] / prefix_sum_array[p][begin];
}

/*===============================  GENERATOR  ================================*/

Multiple<Symbol> PeriodicInhomogeneousMarkovChain::drawSymbol(
    SGPtr<Multiple> generator,
    size_t pos,
    size_t phase,
    const Multiple<Sequence>& context) const {
  auto vlmc = _vlmcs[(pos + phase) % _vlmcs.size()];
  return vlmc->standardGenerator(generator->randomNumberGenerator())
             ->drawSymbol(pos, phase, context);
}

/*----------------------------------------------------------------------------*/

Multiple<Sequence> PeriodicInhomogeneousMarkovChain::drawSequence(
    SGPtr<Multiple> generator,
    size_t size,
    size_t phase) const {
  return Base::drawSequence(generator, size, phase);
}

/*===============================  SERIALIZER  ===============================*/

void PeriodicInhomogeneousMarkovChain::serialize(SSPtr serializer) {
  Base::serialize(serializer);
}

}  // namespace model
}  // namespace tops
