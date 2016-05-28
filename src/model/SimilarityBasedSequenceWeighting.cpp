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
#include "model/SimilarityBasedSequenceWeighting.hpp"

// Standard headers
#include <map>
#include <cmath>
#include <limits>
#include <string>
#include <vector>
#include <sstream>

// Internal headers
#include "model/Util.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

SimilarityBasedSequenceWeighting::SimilarityBasedSequenceWeighting(
    std::map<Sequence, unsigned int> counter,
    double normalizer,
    unsigned int skip_offset,
    unsigned int skip_length,
    Sequence skip_sequence)
      : _counter(counter),
        _skip_offset(skip_offset),
        _skip_length(skip_length),
        _skip_sequence(skip_sequence),
        _normalizer(normalizer) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

SimilarityBasedSequenceWeightingPtr
SimilarityBasedSequenceWeighting::train(TrainerPtr<Standard, Self> trainer,
                                        standard_training_algorithm,
                                        unsigned int alphabet_size,
                                        unsigned int skip_offset,
                                        unsigned int skip_length,
                                        Sequence skip_sequence) {
  auto& training_set = trainer->training_set();

  std::map<Sequence, unsigned int> counter;
  auto min_length = std::numeric_limits<unsigned int>::max();
  for (const auto& training_sequence : training_set) {
    counter[training_sequence]++;

    if (training_sequence.size() < min_length)
      min_length = training_sequence.size();
  }

  auto normalizer = calculate_normalizer(skip_length, skip_offset, min_length,
                                         counter, alphabet_size);

  return SimilarityBasedSequenceWeighting::make(counter, normalizer,
                                                skip_offset, skip_length,
                                                skip_sequence);
}

/*----------------------------------------------------------------------------*/

double SimilarityBasedSequenceWeighting::calculate_normalizer(
    unsigned int skip_length,
    unsigned int skip_offset,
    unsigned int max_length,
    const std::map<Sequence, unsigned int>& counter,
    unsigned int alphabet_size) {
  int npatterns_differ_1 = (alphabet_size-1) * (max_length - skip_length);

  double sum = 0.0;
  for (const auto& count_pair1 : counter) {
    auto count = count_pair1.second;
    sum += count;

    unsigned int diff = 0;
    unsigned int np_differ_1  = 0;

    for (const auto& count_pair2 : counter) {
      const auto& a = count_pair1.first;
      const auto& b = count_pair2.first;

      for (unsigned int i = 0; i < max_length; i++) {
        if ((i >= skip_offset) && (i <= skip_offset + skip_length)) {
          if (a[i] != b[i]) {
            diff += 2;
          }
        } else if (a[i] != b[i]) {
          diff++;
        }
      }
      if (diff == 1) np_differ_1++;
    }

    sum += 0.001 * count * (npatterns_differ_1 - np_differ_1);
  }

  return sum;
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

void SimilarityBasedSequenceWeighting::initializeCache(
    CEPtr<Standard> evaluator, unsigned int phase) {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(evaluator->sequence().size());

  auto sequence_size = evaluator->sequence().size();
  auto simple_evaluator = static_cast<SEPtr<Standard>>(evaluator);

  for (unsigned int i = 0; i < sequence_size; i++)  {
    prefix_sum_array[i]
      = evaluateSequence(simple_evaluator, i, sequence_size, phase);
  }
}

/*----------------------------------------------------------------------------*/

Probability SimilarityBasedSequenceWeighting::evaluateSymbol(
    SEPtr<Standard> /* evaluator */,
    unsigned int /* pos */,
    unsigned int /* phase */) const {
  return 0;  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Probability SimilarityBasedSequenceWeighting::evaluateSequence(
    SEPtr<Standard> evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int /* phase */) const {
  if (end > evaluator->sequence().size()) return 0;

  unsigned int length = _counter.begin()->first.size();

  Sequence ss;
  for (unsigned int i = begin; i < end && i < begin + length; i++)
    ss.push_back(evaluator->sequence()[i]);

  double sum = 0;
  for (auto weight : _counter) {
    int diff = 0;
    if (ss.size() != weight.first.size()) return 0;

    bool valid = true;
    for (unsigned int i = 0; i < weight.first.size(); i++) {
      if ((i >= _skip_offset) && (i < _skip_offset + _skip_length)) {
        if (ss[i] != _skip_sequence[i - _skip_offset]) {
          valid = false;
          break;
        }
      } else if (ss[i] != weight.first[i]) {
        diff++;
      }
    }

    if (!valid) return 0;

    if (diff == 1) {
      sum += 0.001 * weight.second;
    } else if (diff == 0) {
      sum += weight.second;
    }
  }

  if (close(sum, 0.0, 1e-10)) return 0;

  return sum/_normalizer;
}

/*----------------------------------------------------------------------------*/

Probability SimilarityBasedSequenceWeighting::evaluateSequence(
    CEPtr<Standard> evaluator,
    unsigned int begin,
    unsigned int /* end */,
    unsigned int /* phase */) const {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  if (begin < prefix_sum_array.size())
    return prefix_sum_array[begin];
  return 0;
}

/*===============================  GENERATOR  ================================*/

Standard<Symbol> SimilarityBasedSequenceWeighting::drawSymbol(
    SGPtr<Standard> /* generator */,
    unsigned int /* pos */,
    unsigned int /* phase */,
    const Sequence &/* context */) const {
  return Standard<Symbol>(INVALID_SYMBOL);  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
