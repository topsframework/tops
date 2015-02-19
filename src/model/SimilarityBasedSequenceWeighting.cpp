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
#include <string>
#include <sstream>

// ToPS headers
#include "SimilarityBasedSequenceWeighting.hpp"
#include "Util.hpp"

namespace tops {
namespace model {

SimilarityBasedSequenceWeightingPtr SimilarityBasedSequenceWeighting::make(
    int alphabet_size,
    std::map<Sequence, double> counter,
    double normalizer,
    int skip_offset,
    int skip_length,
    Sequence skip_sequence) {
  return SimilarityBasedSequenceWeightingPtr(
    new SimilarityBasedSequenceWeighting(alphabet_size,
                                         counter,
                                         normalizer,
                                         skip_offset,
                                         skip_length,
                                         skip_sequence));
}

SimilarityBasedSequenceWeighting::SimilarityBasedSequenceWeighting(
    int alphabet_size,
    std::map<Sequence, double> counter,
    double normalizer,
    int skip_offset,
    int skip_length,
    Sequence skip_sequence)
      : _counter(counter),
        _skip_offset(skip_offset),
        _skip_length(skip_length),
        _skip_sequence(skip_sequence),
        _normalizer(normalizer),
        _alphabet_size(alphabet_size) {
}

SimilarityBasedSequenceWeightingPtr SimilarityBasedSequenceWeighting::train(
    std::vector<Sequence> training_set,
    unsigned int alphabet_size,
    int skip_offset,
    int skip_length,
    Sequence skip_sequence) {

  std::map<Sequence, double> counter;
  int min_length = 999999999;
  for (int i = 0; i < (int) training_set.size(); i++) {
    if (counter.find(training_set[i]) == counter.end()) {
      counter[training_set[i]] = 1;
    } else {
      counter[training_set[i]] += 1;
    }
    if((int)training_set[i].size() < min_length)
      min_length = training_set[i].size();
  }
  std::string q;
  double normalizer = calculate_normalizer(skip_length, skip_offset, min_length, counter, alphabet_size);

  return SimilarityBasedSequenceWeighting::make(alphabet_size, counter, normalizer, skip_offset, skip_length, skip_sequence);
}

double SimilarityBasedSequenceWeighting::calculate_normalizer(int skip_length, int skip_offset, int max_length, std::map<Sequence, double> & counter, int alphabet_size) {
  int npatterns_differ_1 = 0;
  npatterns_differ_1 = (alphabet_size - 1) * (max_length - skip_length);
  if(skip_length < 0)
    npatterns_differ_1 = (alphabet_size - 1) * (max_length );
  double sum = 0.0;
  for(auto it = counter.begin(); it != counter.end(); it++) {
    sum += it->second;

    int diff = 0;
    int np_differ_1  = 0;

    for(auto it2 = counter.begin(); it2 != counter.end(); it2++) {
      auto a = it->first;
      auto b = it2->first;
      for(int i = 0; i < max_length; i++) {
        if ((i >= skip_offset) && (i <= skip_offset+skip_length)) {
          if (a[i] != b[i])
            diff+=2;
        } else if (a[i] != b[i]) {
          diff++;
        }
      }
      if (diff == 1) {
        np_differ_1 ++;
      }
    }
    sum += 0.001*it->second*(npatterns_differ_1 - np_differ_1);
  }
  return sum;
}

int SimilarityBasedSequenceWeighting::alphabetSize() const {
  return _alphabet_size;
}

double SimilarityBasedSequenceWeighting::evaluatePosition(const Sequence &s, unsigned int i) const {
  return evaluateSequence(s, i, i+1);
}

double SimilarityBasedSequenceWeighting::evaluateSequence(
    const Sequence &s,
    unsigned int begin,
    unsigned int end) const {
  if (end > s.size())
    return -HUGE;
  int length = (_counter.begin()->first).size();

  std::stringstream qstream;
  for (int i = begin; (i < (int)end) && (i < (int) (begin + length)); i++) {
    qstream << std::to_string(s[i]);
  }
  std::string q = qstream.str();
  int psize = 0;
  double sum = 0;
  for (auto it = _counter.begin(); it != _counter.end();  it++) {
    std::stringstream qs2;
    for (auto symbol : it->first)
      qs2 << std::to_string(symbol);
    std::string q2 = qs2.str();
    psize = q2.size();
    int diff = 0;
    if (q.size() != q2.size()) {
      return -HUGE;
    }
    bool valid = true;
    for (int i = 0; i < (int)q2.size(); i++) {
      if ((i >= _skip_offset) && (i < _skip_offset+_skip_length)) {
        if(q[i] != _skip_sequence[i-_skip_offset]){
          valid = false;
          break;
        }
      } else if (q[i] != q2[i]) {
        diff++;
      }
    }
    if (!valid) {
      return -HUGE;
    }
    if (diff == 1) {
      sum += 0.001 * it->second;
    } else if (diff==0) {
      sum += it->second;
    }
  }
  if (close(sum , 0.0, 1e-10)) {
    return -HUGE;
  }
  return log(sum/(_normalizer));
}

Symbol SimilarityBasedSequenceWeighting::choosePosition(const Sequence &s, unsigned int i) const {
  // TODO
  return 0;
}

double SimilarityBasedSequenceWeighting::evaluateWithPrefixSumArray(int begin, int end) {
  if ((begin < (int) _scores.size()) && (begin >= 0)){
    return _scores[begin];
  }
  return -HUGE;
}

void SimilarityBasedSequenceWeighting::initializePrefixSumArray(const Sequence &s) {
  _scores.resize(s.size());
  for (int i = 0; i < (int) s.size(); i++)  {
    _scores[i] = evaluateSequence(s, i, s.size());
  }
}

}  // namespace model
}  // namespace tops
