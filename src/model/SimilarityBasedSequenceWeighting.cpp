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
#include <map>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>

// ToPS headers
#include "SimilarityBasedSequenceWeighting.hpp"
#include "Util.hpp"

namespace tops {
namespace model {

SimilarityBasedSequenceWeightingPtr SimilarityBasedSequenceWeighting::make(
    std::map<Sequence, double> counter,
    double normalizer,
    int skip_offset,
    int skip_length,
    Sequence skip_sequence) {
  return SimilarityBasedSequenceWeightingPtr(
    new SimilarityBasedSequenceWeighting(counter,
                                         normalizer,
                                         skip_offset,
                                         skip_length,
                                         skip_sequence));
}

SimilarityBasedSequenceWeighting::SimilarityBasedSequenceWeighting(
    std::map<Sequence, double> counter,
    double normalizer,
    int skip_offset,
    int skip_length,
    Sequence skip_sequence)
      : _counter(counter),
        _skip_offset(skip_offset),
        _skip_length(skip_length),
        _skip_sequence(skip_sequence),
        _normalizer(normalizer) {
}

SimilarityBasedSequenceWeightingPtr SimilarityBasedSequenceWeighting::train(
    std::vector<Sequence> training_set,
    unsigned int alphabet_size,
    int skip_offset,
    int skip_length,
    Sequence skip_sequence) {

  std::map<Sequence, double> counter;
  unsigned int min_length = 999999999;
  for (unsigned int i = 0; i < training_set.size(); i++) {
    if (counter.find(training_set[i]) == counter.end()) {
      counter[training_set[i]] = 1;
    } else {
      counter[training_set[i]] += 1;
    }
    if (training_set[i].size() < min_length)
      min_length = training_set[i].size();
  }
  double normalizer = calculate_normalizer(skip_length, skip_offset, min_length,
                                           counter, alphabet_size);

  return SimilarityBasedSequenceWeighting::make(counter,
                                                normalizer, skip_offset,
                                                skip_length, skip_sequence);
}

double SimilarityBasedSequenceWeighting::calculate_normalizer(
    int skip_length,
    int skip_offset,
    int max_length,
    std::map<Sequence, double> & counter,
    int alphabet_size) {
  int npatterns_differ_1 = 0;
  npatterns_differ_1 = (alphabet_size - 1) * (max_length - skip_length);
  if (skip_length < 0)
    npatterns_differ_1 = (alphabet_size - 1) * (max_length);
  double sum = 0.0;
  for (auto it = counter.begin(); it != counter.end(); it++) {
    sum += it->second;

    int diff = 0;
    int np_differ_1  = 0;

    for (auto it2 = counter.begin(); it2 != counter.end(); it2++) {
      auto a = it->first;
      auto b = it2->first;
      for (int i = 0; i < max_length; i++) {
        if ((i >= skip_offset) && (i <= skip_offset+skip_length)) {
          if (a[i] != b[i])
            diff+=2;
        } else if (a[i] != b[i]) {
          diff++;
        }
      }
      if (diff == 1)
        np_differ_1++;
    }
    sum += 0.001*it->second*(npatterns_differ_1 - np_differ_1);
  }
  return sum;
}

double SimilarityBasedSequenceWeighting::evaluate(const Sequence &s,
                                                  unsigned int pos,
                                                  unsigned int phase) const {
  // TODO(igorbonadio)
  return -HUGE;
}

Symbol SimilarityBasedSequenceWeighting::choose(const Sequence &context,
                                                unsigned int pos,
                                                unsigned int phase) const {
  // TODO(igorbonadio)
  return 0;
}

EvaluatorPtr SimilarityBasedSequenceWeighting::evaluator(const Sequence &s,
                                                        bool cached) {
  // if (cached)
  //   return Evaluator::make(
  //     CachedEvaluatorImpl<SimilarityBasedSequenceWeighting>::make(
  //       std::static_pointer_cast<SimilarityBasedSequenceWeighting>(shared_from_this()),
  //       s, Cache(s.size())));
  return Evaluator::make(
    SimpleEvaluatorImpl<SimilarityBasedSequenceWeighting>::make(
      std::static_pointer_cast<SimilarityBasedSequenceWeighting>(shared_from_this()),
      s));
}

double SimilarityBasedSequenceWeighting::simpleProbabilityOf(
    SEPtr evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int phase) const {
  if (end > evaluator->sequence().size())
    return -HUGE;
  int length = (_counter.begin()->first).size();

  Sequence ss;
  for (unsigned int i = begin; i < end && i < begin + length; i++)
    ss.push_back(evaluator->sequence()[i]);

  double sum = 0;
  for (auto weight : _counter) {
    int diff = 0;
    if (ss.size() != weight.first.size())
      return -HUGE;

    bool valid = true;
    for (unsigned int i = 0; i < weight.first.size(); i++) {
      if ((i >= _skip_offset) && (i < _skip_offset+_skip_length)) {
        if (ss[i] != _skip_sequence[i-_skip_offset]) {
          valid = false;
          break;
        }
      } else if (ss[i] != weight.first[i]) {
        diff++;
      }
    }

    if (!valid)
      return -HUGE;

    if (diff == 1) {
      sum += 0.001 * weight.second;
    } else if (diff == 0) {
      sum += weight.second;
    }
  }

  if (close(sum , 0.0, 1e-10))
    return -HUGE;

  return log(sum/(_normalizer));
}

void SimilarityBasedSequenceWeighting::initializeCachedEvaluator(
    CEPtr evaluator,
    unsigned int phase) {
  auto &prefix_sum_array = evaluator->cache();
  for (unsigned int i = 0; i < evaluator->sequence().size(); i++)  {
    prefix_sum_array[i] = simpleProbabilityOf(evaluator, i, evaluator->sequence().size());
  }
}

double SimilarityBasedSequenceWeighting::cachedProbabilityOf(
    CEPtr evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int phase) const {
  auto &prefix_sum_array = evaluator->cache();
  if (begin < prefix_sum_array.size())
    return prefix_sum_array[begin];
  return -HUGE;
}

}  // namespace model
}  // namespace tops
