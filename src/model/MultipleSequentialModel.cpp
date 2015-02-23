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

#include <stdlib.h>

// ToPS headers
#include "MultipleSequentialModel.hpp"

#include "Util.hpp"

namespace tops {
namespace model {

MultipleSequentialModelPtr MultipleSequentialModel::make(
      std::vector<FactorableModelPtr> models,
      std::vector<int> max_length) {
  return MultipleSequentialModelPtr(new MultipleSequentialModel(models, max_length));
}

MultipleSequentialModel::MultipleSequentialModel(
    std::vector<FactorableModelPtr> models,
    std::vector<int> max_length) : _models(models),
                                            _max_length(max_length) {

  _idx_not_limited = _models.size() - 1;
  int count = 0;
  for (unsigned int i = 0; i < _models.size(); i++) {
    if (_max_length[i] < 0) {
      count++;
      _idx_not_limited = i;
    }
  }
  if (count > 1) {
    // std::cerr << "ERROR: Only one model can has unlimited length\n" << std::endl;
    // TODO
    exit(-1);
  }
}

int MultipleSequentialModel::alphabetSize() const {
  return _models[0]->alphabetSize();
}

double MultipleSequentialModel::evaluateSequence(const Sequence &s,
                        unsigned int begin,
                        unsigned int end) const {
  if(begin > end)
    return -HUGE;

  unsigned int phase = 0;

  double sum = 0;
  int b = begin;
  int e = 0;

  for (int i = 0; i < _idx_not_limited; i++) {
    e = b + _max_length[i] - 1;
    if (e >= s.size())
      e = s.size()-1;
    sum += _models[i]->evaluateSequence(s,b,e/*,phase*/);
    if (e >= (int)end)
      return sum;

    phase = mod(phase + e - b + 1, 3);
    b = e + 1;
    if(e >= s.size())
      break;
  }
  int begin_of_not_limited = b;
  e = end;
  for (int i = _models.size()-1; i > _idx_not_limited ; i--) {
    b = e - _max_length[i] + 1;
    unsigned int phase2 = mod(phase + b - begin_of_not_limited, 3);
    if (b < 0) {
      phase2 = mod(phase2 -b, 3);
      b  = 0;
    }
    sum += _models[i]->evaluateSequence(s,b,e/*,phase2*/);
    e = b - 1;
    if (e < 0)
      break;

  }
  int end_of_not_limited = e;
  if (end_of_not_limited - begin_of_not_limited + 1 > 0 ) {
    sum += _models[_idx_not_limited]->evaluateSequence(s,begin_of_not_limited, end_of_not_limited/*, phase*/);
  }
  return sum;
}

double MultipleSequentialModel::evaluatePosition(const Sequence &s, unsigned int i) const {
  return evaluateSequence(s, i, i);
}

Symbol MultipleSequentialModel::choosePosition(const Sequence &s, unsigned int i) const {
  int index = i;
  for (unsigned int j = 0; j < _models.size(); j++) {
    index -= _max_length[j];
    if (index < 0)
      return _models[j]->choosePosition(s, i);
  }
  return _models.back()->choosePosition(s, i);
}

double MultipleSequentialModel::evaluateWithPrefixSumArray(unsigned int begin,
                                                           unsigned int end) {
  unsigned int phase = 0;
  double sum = 0;
  int b = begin;
  int e = 0;
  for (int i = 0; i < _idx_not_limited; i++) {
    e = b + _max_length[i] - 1;
    if (e >= _seqsize)
      e = _seqsize-1;
    sum += _models[i]->evaluateWithPrefixSumArray(b,e/*,phase*/);
    if( e >=  (int)end)
      return sum;

    phase = mod(phase + e - b + 1, 3);
    b = e + 1;
    if(e>=_seqsize)
      break;
  }
  int begin_of_not_limited = b;
  e = end;
  for (int i = _models.size()-1; i > _idx_not_limited ; i--) {
    b = e - _max_length[i] + 1;
    int phase2 = mod(phase + b - begin_of_not_limited, 3);
    if (b < 0) {
      phase2 = mod(phase2 -b, 3);
      b  = 0;
    }
    sum += _models[i]->evaluateWithPrefixSumArray(b,e/*,phase2*/);
    e = b - 1;
    if (e < 0)
      break;
  }
  int end_of_not_limited = e;
  if (end_of_not_limited - begin_of_not_limited + 1 > 0 ) {
    sum += _models[_idx_not_limited]->evaluateWithPrefixSumArray(begin_of_not_limited, end_of_not_limited/*, phase*/);
  }
  return sum;
}

void MultipleSequentialModel::initializePrefixSumArray(const Sequence &s) {
  for (int i = 0; i < (int)_models.size(); i++)
    _models[i]->initializePrefixSumArray(s);
  _seqsize = s.size();
}

}  // namespace model
}  // namespace tops
