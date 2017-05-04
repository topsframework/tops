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
#include "model/InhomogeneousMarkovChain.hpp"

// Standard headers
#include <cmath>
#include <limits>
#include <vector>
#include <iostream>

// Internal headers
#include "exception/OutOfRange.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

InhomogeneousMarkovChain::InhomogeneousMarkovChain(
    std::vector<VariableLengthMarkovChainPtr> vlmcs)
    : _vlmcs(vlmcs) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

/*================================  TRAINER  =================================*/

InhomogeneousMarkovChainPtr InhomogeneousMarkovChain::train(
    TrainerPtr<Standard, Self> trainer,
    wam_algorithm,
    unsigned int alphabet_size,
    unsigned int order,
    unsigned int length,
    unsigned int offset,
    unsigned int vicinity,
    double pseudo_counts,
    Sequence fixed_sequence,
    unsigned int fixed_sequence_position,
    std::vector<double> weights) {

  auto& sample_set = trainer->training_set();





  std::vector<VariableLengthMarkovChainPtr> positional_distribution;
  positional_distribution.resize(length);
  bool fixseq = false;
  Sequence fixed;
  int fixed_pos = 0;
  if (fixed_sequence.size() > 0) {
    fixed = fixed_sequence;
    fixed_pos = fixed_sequence_position;
    fixseq = true;
  }

  for(auto i = 0u; i < length; i++) {
    std::vector<Sequence> positionalSample;
    std::vector<double> w;
    auto o = i;
    if (o > order)
      o = order;
    for(auto j = 0u; j < sample_set.size(); j++) {
      for(int k = -vicinity; k <= static_cast<int>(vicinity); k++) {
        Sequence s;
        s.resize(o+1);
        unsigned int l = 0;
        int m = i - o + k + offset;
        if(m < 0)
          continue;
        if((m + o) >= sample_set[j].size())
          continue;
        if(fixseq) {
          while((m < static_cast<int>(sample_set[j].size())) && (l <= o)) {
            s[l] = sample_set[j][m];
            if(fixseq && (fixed_pos <= (m-k)) && ( (m-k) <= static_cast<int>(fixed_pos + fixed.size()-1))) {
              int p = m - fixed_pos - k;
              if((p >= 0) && (p < static_cast<int>(fixed.size())))
                s[l] = fixed[p];
            }
            l++;
            m++;
          }
        } else {
          while( (m < static_cast<int>(sample_set[j].size())) && (l <= o)) {
            s[l] = sample_set[j][m];
            l++;
            m++;
          }
        }
        positionalSample.push_back(s);
        w.push_back(weights[j]);
      }
    }

    if(fixseq && (fixed_pos <= static_cast<int>(i)) && (static_cast<unsigned long>(i) <= (fixed_pos + fixed.size() - 1))){
      ContextTreePtr tree = ContextTree::make(alphabet_size);
      tree->initializeCounter(positionalSample, o, pseudo_counts, w);
      tree->normalize();
      positional_distribution[i] = VariableLengthMarkovChain::make(tree);
    } else {
      ContextTreePtr tree = ContextTree::make(alphabet_size);
      tree->initializeCounter(positionalSample, o, pseudo_counts, w);
      tree->normalize();
      positional_distribution[i] = VariableLengthMarkovChain::make(tree);
    }

  }

  return InhomogeneousMarkovChain::make(positional_distribution);
}


/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

Probability
InhomogeneousMarkovChain::evaluateSymbol(SEPtr<Standard> evaluator,
                                         unsigned int pos,
                                         unsigned int phase) const {
  return _vlmcs[phase]->standardEvaluator(evaluator->sequence())
                      ->evaluateSymbol(pos);
}

/*----------------------------------------------------------------------------*/

Probability InhomogeneousMarkovChain::evaluateSequence(
    SEPtr<Standard> evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int /*phase*/) const {
  if ((end - begin) > _vlmcs.size())
    return 0.0;
  auto t = 0u;
  Probability prob = 1;
  for (unsigned int i = begin; i < end; i++) {
    prob *= evaluator->evaluateSymbol(i, t);
    t++;
  }
  return prob;
}

/*----------------------------------------------------------------------------*/

void InhomogeneousMarkovChain::initializeCache(CEPtr<Standard> evaluator,
                                               unsigned int /*phase*/) {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(_vlmcs.size());
  for (auto k = 0u; k < _vlmcs.size(); k++) {
    prefix_sum_array[k].resize(evaluator->sequence().size() + 1);
    prefix_sum_array[k][0] = 1.0;
    for (auto i = 0u; i < evaluator->sequence().size(); i++) {
      auto phase = (i + k) % _vlmcs.size();
      prefix_sum_array[k][i + 1] = prefix_sum_array[k][i] * evaluator->evaluateSymbol(i, phase);
    }
  }
}

/*----------------------------------------------------------------------------*/

Probability
InhomogeneousMarkovChain::evaluateSymbol(CEPtr<Standard> evaluator,
                                         unsigned int pos,
                                         unsigned int phase) const {
  return _vlmcs[phase]->standardEvaluator(evaluator->sequence())
                      ->evaluateSymbol(pos);
}

/*----------------------------------------------------------------------------*/

Probability InhomogeneousMarkovChain::evaluateSequence(
    CEPtr<Standard> evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int phase) const {
  if ((end - begin) > _vlmcs.size())
    return 0.0;

  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;

  auto p = begin % _vlmcs.size();
  auto k = 0u;
  while (((p + k) % _vlmcs.size()) != phase)
    k++;
  p = k;

  return prefix_sum_array[p][end] / prefix_sum_array[p][begin];
}

/*===============================  GENERATOR  ================================*/

Standard<Symbol>
InhomogeneousMarkovChain::drawSymbol(SGPtr<Standard> generator,
                                     unsigned int pos,
                                     unsigned int phase,
                                     const Sequence& context) const {
  if (pos + phase < _vlmcs.size()) {
    auto vlmc = _vlmcs[pos + phase];
    return vlmc->standardGenerator(generator->randomNumberGenerator())
               ->drawSymbol(pos, phase, context);
  }

  throw_exception(OutOfRange);
}

/*----------------------------------------------------------------------------*/

Standard<Sequence> InhomogeneousMarkovChain::drawSequence(
    SGPtr<Standard> generator,
    unsigned int size,
    unsigned int phase) const {
  return Base::drawSequence(generator, size, phase);
}

/*===============================  SERIALIZER  ===============================*/

void InhomogeneousMarkovChain::serialize(SSPtr serializer) {
  Base::serialize(serializer);
}

/*----------------------------------------------------------------------------*/
/*                             VIRTUAL METHODS                                */
/*----------------------------------------------------------------------------*/

unsigned int InhomogeneousMarkovChain::maximumTimeValue() {
  return _vlmcs.size();
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
