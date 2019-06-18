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

#include "exception/NotYetImplemented.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

SimilarityBasedSequenceWeighting::SimilarityBasedSequenceWeighting(
    std::map<Sequence, size_t> counter,
    double normalizer,
    size_t skip_offset,
    size_t skip_length,
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
SimilarityBasedSequenceWeighting::train(TrainerPtr<Multiple, Self> trainer,
                                        standard_training_algorithm,
                                        size_t alphabet_size,
                                        size_t skip_offset,
                                        size_t skip_length,
                                        Sequence skip_sequence) {
  std::map<Sequence, size_t> counter;
  auto min_length = std::numeric_limits<size_t>::max();
  for (const auto& sequences : trainer->training_set()) {
    counter[sequences[0]]++;

    if (sequences[0].size() < min_length)
      min_length = sequences[0].size();
  }

  auto normalizer = calculate_normalizer(skip_length, skip_offset, min_length,
                                         counter, alphabet_size);

  return SimilarityBasedSequenceWeighting::make(counter, normalizer,
                                                skip_offset, skip_length,
                                                skip_sequence);
}

/*----------------------------------------------------------------------------*/

double SimilarityBasedSequenceWeighting::calculate_normalizer(
    size_t skip_length,
    size_t skip_offset,
    size_t max_length,
    const std::map<Sequence, size_t>& counter,
    size_t alphabet_size) {
  int npatterns_differ_1 = (alphabet_size-1) * (max_length - skip_length);

  double sum = 0.0;
  for (const auto& count_pair1 : counter) {
    auto count = count_pair1.second;
    sum += count;

    size_t diff = 0;
    size_t np_differ_1  = 0;

    for (const auto& count_pair2 : counter) {
      const auto& a = count_pair1.first;
      const auto& b = count_pair2.first;

      for (size_t i = 0; i < max_length; i++) {
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

Probability SimilarityBasedSequenceWeighting::evaluateSymbol(
    SEPtr<Multiple> /* evaluator */,
    size_t /* pos */,
    size_t /* phase */) const {
  throw_exception(NotYetImplemented);
}

/*----------------------------------------------------------------------------*/

Probability SimilarityBasedSequenceWeighting::evaluateSequence(
    SEPtr<Multiple> evaluator,
    size_t begin,
    size_t end,
    size_t /* phase */) const {
  if (end > evaluator->sequence()[0].size()) return 0;

  size_t length = _counter.begin()->first.size();

  Sequence ss;
  for (size_t i = begin; i < end && i < begin + length; i++)
    ss.push_back(evaluator->sequence()[0][i]);

  double sum = 0;
  for (auto weight : _counter) {
    int diff = 0;
    if (ss.size() != weight.first.size()) return 0;

    bool valid = true;
    for (size_t i = 0; i < weight.first.size(); i++) {
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

void SimilarityBasedSequenceWeighting::initializeCache(
    CEPtr<Multiple> evaluator, size_t phase) {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(evaluator->sequence()[0].size());

  auto sequence_size = evaluator->sequence()[0].size();
  auto simple_evaluator = static_cast<SEPtr<Multiple>>(evaluator);

  for (size_t i = 0; i < sequence_size; i++)  {
    prefix_sum_array[i]
      = evaluateSequence(simple_evaluator, i, sequence_size, phase);
  }
}

/*----------------------------------------------------------------------------*/

Probability SimilarityBasedSequenceWeighting::evaluateSymbol(
    CEPtr<Multiple> evaluator,
    size_t pos,
    size_t phase) const {
  return Base::evaluateSymbol(evaluator, pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability SimilarityBasedSequenceWeighting::evaluateSequence(
    CEPtr<Multiple> evaluator,
    size_t begin,
    size_t /* end */,
    size_t /* phase */) const {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  if (begin < prefix_sum_array.size())
    return prefix_sum_array[begin];
  return 0;
}

/*===============================  GENERATOR  ================================*/

Multiple<Symbol> SimilarityBasedSequenceWeighting::drawSymbol(
    SGPtr<Multiple> /* generator */,
    size_t /* pos */,
    size_t /* phase */,
    const Multiple<Sequence>&/* context */) const {
  throw_exception(NotYetImplemented);
}

/*----------------------------------------------------------------------------*/

Multiple<Sequence> SimilarityBasedSequenceWeighting::drawSequence(
    SGPtr<Multiple> generator,
    size_t size,
    size_t phase) const {
  return Base::drawSequence(generator, size, phase);
}

/*===============================  SERIALIZER  ===============================*/

void SimilarityBasedSequenceWeighting::serialize(SSPtr serializer) {
  Base::serialize(serializer);
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
