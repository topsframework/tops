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
    std::vector<Sequence> training_set_1,
    std::vector<Sequence> training_set_2,
    unsigned int alphabet_size,
    unsigned int order,
    unsigned int nphases,
    double pseudo_counts,
    std::vector<double> weights,
    ProbabilisticModelPtr apriori) {


  int length = nphases;
  std::vector<ContextTreePtr> positional_distribution;
  std::vector<VariableLengthMarkovChainPtr> vlmcs;
  positional_distribution.resize(length);
  vlmcs.resize(length);
  int sample_size = 0;

  for (int i = 0; i < length; i++) {
    std::vector<Sequence> positionalSample;
    for (int j = 0; j < static_cast<int>(training_set.size()); j++) {
      int nseq = 0;
      while (true) {
        int start = (length) * nseq - order + i;
        if (start < 0) {
          nseq++;
          continue;
        }
        int end = (length) * nseq + i;
        if (end >= static_cast<int>(training_set[j].size()))
          break;
        Sequence s;
        for (int k = start; k <= end; k++)
          s.push_back(training_set[j][k]);
        positionalSample.push_back(s);
        nseq++;
      }
    }

    for (int j = 0; j < static_cast<int>(training_set_1.size()); j++) {
      int nseq = 0;
      while (true) {
        int start = (length) * nseq - order + i -1;
        if (start < 0) {
          nseq++;
          continue;
        }
        int end = (length) * nseq + i -1;
        if (end >= static_cast<int>(training_set_1[j].size()))
          break;
        Sequence s;
        for (int k = start; k <= end; k++)
          s.push_back(training_set_1[j][k]);
        positionalSample.push_back(s);
        nseq++;
      }
    }

    for (int j = 0; j < static_cast<int>(training_set_2.size()); j++) {
      int nseq = 0;
      while (true) {
        int start = (length) * nseq - order + i -2;
        if (start < 0) {
          nseq++;
          continue;
        }
        int end = (length) * nseq + i -2;
        if (end >= static_cast<int>(training_set_2[j].size()))
          break;
        Sequence s;
        for (int k = start; k <= end; k++)
          s.push_back(training_set_2[j][k]);
        positionalSample.push_back(s);
        nseq++;
      }
    }

    ContextTreePtr tree = ContextTree::make(alphabet_size);

    if (apriori) {
      tree->initializeCounter(positionalSample, order, 0, weights);
      tree->pruneTreeSmallSampleSize(400);
      // TODO(igorbonadio): check normalize(3-params).
      tree->normalize(apriori, pseudo_counts/*, i*/);
    } else {
      tree->initializeCounter(positionalSample, order, pseudo_counts, weights);
      tree->pruneTreeSmallSampleSize(400);
      tree->normalize();
    }
    positional_distribution[i] = tree;
    vlmcs[i] = VariableLengthMarkovChain::make(tree);
  }

  auto model = PhasedInhomogeneousMarkovChain::make(vlmcs);
  double loglikelihood = 0.0;

  for (int j = 0; j < static_cast<int>(training_set.size()); j++) {
    sample_size += training_set[j].size();
    loglikelihood += model->evaluateSequence(training_set[j],
                                             0,
                                             training_set[j].size() - 1);
  }
  return model;
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
