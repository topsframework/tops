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
PeriodicInhomogeneousMarkovChain::train(TrainerPtr<Standard, Self> trainer,
                                        interpolation_algorithm,
                                        unsigned int alphabet_size,
                                        unsigned int order,
                                        unsigned int nphases,
                                        double pseudo_counts,
                                        std::vector<double> weights,
                                        ProbabilisticModelPtr apriori) {
  auto& training_set = trainer->training_set();

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

  return PeriodicInhomogeneousMarkovChain::make(vlmcs);
}

/*=================================  OTHERS  =================================*/

PeriodicInhomogeneousMarkovChainPtr PeriodicInhomogeneousMarkovChain::make(
    std::vector<VariableLengthMarkovChainPtr> vlmcs) {
  return PeriodicInhomogeneousMarkovChainPtr(
    new PeriodicInhomogeneousMarkovChain(vlmcs));
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

void PeriodicInhomogeneousMarkovChain::initializeCache(
    CEPtr<Standard> evaluator, unsigned int /* phase */) {
  auto& prefix_sum_matrix = evaluator->cache().prefix_sum_matrix;

  prefix_sum_matrix.resize(_vlmcs.size());
  for (auto& prefix_sum_array : prefix_sum_matrix)
    prefix_sum_array.resize(evaluator->sequence().size() + 1);

  auto simple_evaluator = static_cast<SEPtr<Standard>>(evaluator);

  for (unsigned int t = 0; t < _vlmcs.size() ; t++) {
    prefix_sum_matrix[t][0] = 0;
    for (unsigned int i = 0; i < evaluator->sequence().size(); i++) {
      prefix_sum_matrix[t][i+1] = prefix_sum_matrix[t][i]
        + evaluateSymbol(simple_evaluator, i, t);
    }
  }
}

/*----------------------------------------------------------------------------*/

Probability
PeriodicInhomogeneousMarkovChain::evaluateSymbol(SEPtr<Standard> evaluator,
                                                 unsigned int pos,
                                                 unsigned int phase) const {
  return _vlmcs[(pos + phase) % _vlmcs.size()]
    ->standardEvaluator(evaluator->sequence())->evaluateSymbol(pos);
}

/*----------------------------------------------------------------------------*/

Probability
PeriodicInhomogeneousMarkovChain::evaluateSequence(SEPtr<Standard> evaluator,
                                                   unsigned int begin,
                                                   unsigned int end,
                                                   unsigned int phase) const {
  Probability prob = 0;
  for (unsigned int i = begin; i < end; i++)
    prob += evaluator->evaluateSymbol(i, phase);
  return prob;
}

/*----------------------------------------------------------------------------*/

Probability
PeriodicInhomogeneousMarkovChain::evaluateSequence(CEPtr<Standard> evaluator,
                                                   unsigned int begin,
                                                   unsigned int end,
                                                   unsigned int phase) const {
  auto& prefix_sum_matrix = evaluator->cache().prefix_sum_matrix;
  return prefix_sum_matrix[phase][end] - prefix_sum_matrix[phase][begin];
}

/*===============================  GENERATOR  ================================*/

Standard<Symbol>
PeriodicInhomogeneousMarkovChain::drawSymbol(SGPtr<Standard> generator,
                                             unsigned int pos,
                                             unsigned int phase,
                                             const Sequence& context) const {
  auto vlmc = _vlmcs[(pos + phase) % _vlmcs.size()];
  return vlmc->standardGenerator(generator->randomNumberGenerator())
             ->drawSymbol(pos, phase, context);
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
