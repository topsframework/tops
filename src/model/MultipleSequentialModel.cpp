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
#include <cstdlib>

// ToPS headers
#include "MultipleSequentialModel.hpp"

#include "Util.hpp"

namespace tops {
namespace model {

MultipleSequentialModelPtr MultipleSequentialModel::make(
      std::vector<ProbabilisticModelPtr> models,
      std::vector<int> max_length) {
  return MultipleSequentialModelPtr(
    new MultipleSequentialModel(models, max_length));
}

MultipleSequentialModel::MultipleSequentialModel(
    std::vector<ProbabilisticModelPtr> models,
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
    // std::cerr << "ERROR: Only one model can has unlimited length\n"
    // << std::endl;
    // TODO(igorbonadio)
    exit(-1);
  }
}

double MultipleSequentialModel::evaluateSequence(const Sequence &s,
                        unsigned int begin,
                        unsigned int end,
                        unsigned int phase) const {
  if (begin > end)
    return -HUGE;

  double sum = 0;
  int b = begin;
  int e = 0;

  for (unsigned int i = 0; i < _idx_not_limited; i++) {
    e = b + _max_length[i] - 1;
    if (e >= static_cast<int>(s.size()))
      e = s.size()-1;
    sum += _models[i]->evaluateSequence(s, b, e, phase);
    if (e >= static_cast<int>(end))
      return sum;

    phase = mod(phase + e - b + 1, 3);
    b = e + 1;
    if (e >= static_cast<int>(s.size()))
      break;
  }
  int begin_of_not_limited = b;
  e = end;
  for (unsigned int i = _models.size()-1; i > _idx_not_limited ; i--) {
    b = e - _max_length[i] + 1;
    unsigned int phase2 = mod(phase + b - begin_of_not_limited, 3);
    if (b < 0) {
      phase2 = mod(phase2 -b, 3);
      b  = 0;
    }
    sum += _models[i]->evaluateSequence(s, b, e, phase2);
    e = b - 1;
    if (e < 0)
      break;
  }
  int end_of_not_limited = e;
  if (end_of_not_limited - begin_of_not_limited + 1 > 0)
    sum += _models[_idx_not_limited]->evaluateSequence(
        s, begin_of_not_limited, end_of_not_limited, phase);
  return sum;
}

double MultipleSequentialModel::evaluatePosition(const Sequence &s,
                                                 unsigned int i,
                                                 unsigned int phase) const {
  return evaluateSequence(s, i, i, phase);
}

Symbol MultipleSequentialModel::choosePosition(const Sequence &s,
                                               unsigned int i,
                                               unsigned int phase) const {
  int index = i;
  for (unsigned int j = 0; j < _models.size(); j++) {
    index -= _max_length[j];
    if (index < 0)
      return _models[j]->choosePosition(s, i);
  }
  return _models.back()->choosePosition(s, i);
}

EvaluatorPtr MultipleSequentialModel::evaluate(const Sequence &s,
                                               bool cached) {
  if (cached)
    return std::static_pointer_cast<Evaluator>(
        CachedEvaluator<MultipleSequentialModel>::make(
          std::static_pointer_cast<MultipleSequentialModel>(shared_from_this()),
          s,
          cache(_models.size())));
  return std::static_pointer_cast<Evaluator>(
      SimpleEvaluator<MultipleSequentialModel>::make(
        std::static_pointer_cast<MultipleSequentialModel>(shared_from_this()),
        s));
}

void MultipleSequentialModel::initializePrefixSumArray(
    CEPtr evaluator,
    unsigned int phase) {
  auto &evaluators = evaluator->memory();
  for (unsigned int i = 0; i < _models.size(); i++)
    evaluators[i] = _models[i]->evaluate(evaluator->sequence, true);
}

double MultipleSequentialModel::evaluateWithPrefixSumArray(
    CEPtr evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int phase) const {
  auto &evaluators = evaluator->memory();
  double sum = 0;
  int b = begin;
  int e = 0;
  for (unsigned int i = 0; i < _idx_not_limited; i++) {
    e = b + _max_length[i] - 1;
    if (e >= static_cast<int>(evaluator->sequence.size()))
      e = evaluator->sequence.size()-1;
    sum += evaluators[i]->probabilityOf(b, e, phase);
    if (e >= static_cast<int>(end))
      return sum;

    phase = mod(phase + e - b + 1, 3);
    b = e + 1;
    if (e >= static_cast<int>(evaluator->sequence.size()))
      break;
  }
  int begin_of_not_limited = b;
  e = end;
  for (unsigned int i = _models.size()-1; i > _idx_not_limited ; i--) {
    b = e - _max_length[i] + 1;
    int phase2 = mod(phase + b - begin_of_not_limited, 3);
    if (b < 0) {
      phase2 = mod(phase2 -b, 3);
      b  = 0;
    }
    sum += evaluators[i]->probabilityOf(b, e, phase2);
    e = b - 1;
    if (e < 0)
      break;
  }
  int end_of_not_limited = e;
  if (end_of_not_limited - begin_of_not_limited + 1 > 0) {
    sum += evaluators[_idx_not_limited]->probabilityOf(
        begin_of_not_limited, end_of_not_limited, phase);
  }
  return sum;
}

}  // namespace model
}  // namespace tops
