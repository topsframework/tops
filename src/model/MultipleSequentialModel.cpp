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
#include <limits>
#include <vector>
#include <cstdlib>

// ToPS headers
#include "Util.hpp"

// Interface header
#include "MultipleSequentialModel.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

MultipleSequentialModel::MultipleSequentialModel(
std::vector<ProbabilisticModelPtr> models,
  std::vector<int> max_length)
    : _models(models), _max_length(max_length) {
  _idx_not_limited = _models.size() - 1;
  int count = 0;
  for (unsigned int i = 0; i < _models.size(); i++) {
    if (_max_length[i] < 0) {
      count++;
      _idx_not_limited = i;
    }
  }
  if (count > 1) {
    // TODO(igorbonadio): ERROR: Only one model can has unlimited length
    exit(-1);
  }
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

MultipleSequentialModelPtr MultipleSequentialModel::make(
      std::vector<ProbabilisticModelPtr> models,
      std::vector<int> max_length) {
  return MultipleSequentialModelPtr(
    new MultipleSequentialModel(models, max_length));
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

void MultipleSequentialModel::initializeCache(CEPtr<Standard> evaluator,
                                              unsigned int /* phase */) {
  evaluator->cache().evaluators.resize(_models.size());

  for (unsigned int i = 0; i < _models.size(); i++)
    evaluator->cache().evaluators[i]
      = _models[i]->standardEvaluator(evaluator->sequence(), true);
}

/*----------------------------------------------------------------------------*/

Probability
MultipleSequentialModel::evaluateSymbol(SEPtr<Standard> /* evaluator */,
                                        unsigned int /* pos */,
                                        unsigned int /* phase */) const {
  // TODO(igorbonadio)
  return -std::numeric_limits<Probability>::infinity();
}

/*----------------------------------------------------------------------------*/

Probability
MultipleSequentialModel::evaluateSequence(SEPtr<Standard> evaluator,
                                          unsigned int begin,
                                          unsigned int end,
                                          unsigned int phase) const {
  if (begin > end)
    return -std::numeric_limits<Probability>::infinity();

  Probability sum = 0;
  int b = begin;
  int e = 0;

  for (unsigned int i = 0; i < _idx_not_limited; i++) {
    e = b + _max_length[i] - 1;
    if (e >= static_cast<int>(evaluator->sequence().size()))
      e = evaluator->sequence().size()-1;
    sum += _models[i]->standardEvaluator(evaluator->sequence())
      ->evaluateSequence(b, e, phase);
    if (e >= static_cast<int>(end))
      return sum;

    phase = mod(phase + e - b + 1, 3);
    b = e + 1;
    if (e >= static_cast<int>(evaluator->sequence().size()))
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
    sum += _models[i]->standardEvaluator(evaluator->sequence())
      ->evaluateSequence(b, e, phase2);
    e = b - 1;
    if (e < 0)
      break;
  }
  int end_of_not_limited = e;
  if (end_of_not_limited - begin_of_not_limited + 1 > 0)
    sum += _models[_idx_not_limited]
      ->standardEvaluator(evaluator->sequence())->evaluateSequence(
        begin_of_not_limited, end_of_not_limited, phase);
  return sum;
}

/*----------------------------------------------------------------------------*/

Probability
MultipleSequentialModel::evaluateSequence(CEPtr<Standard> evaluator,
                                          unsigned int begin,
                                          unsigned int end,
                                          unsigned int phase) const {
  auto &evaluators = evaluator->cache().evaluators;
  double sum = 0;
  int b = begin;
  int e = 0;
  for (unsigned int i = 0; i < _idx_not_limited; i++) {
    e = b + _max_length[i] - 1;
    if (e >= static_cast<int>(evaluator->sequence().size()))
      e = evaluator->sequence().size()-1;
    sum += evaluators[i]->evaluateSequence(b, e, phase);
    if (e >= static_cast<int>(end))
      return sum;

    phase = mod(phase + e - b + 1, 3);
    b = e + 1;
    if (e >= static_cast<int>(evaluator->sequence().size()))
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
    sum += evaluators[i]->evaluateSequence(b, e, phase2);
    e = b - 1;
    if (e < 0)
      break;
  }
  int end_of_not_limited = e;
  if (end_of_not_limited - begin_of_not_limited + 1 > 0) {
    sum += evaluators[_idx_not_limited]->evaluateSequence(
        begin_of_not_limited, end_of_not_limited, phase);
  }
  return sum;
}

/*===============================  GENERATOR  ================================*/

Standard<Symbol>
MultipleSequentialModel::drawSymbol(SGPtr<Standard> generator,
                                    unsigned int pos,
                                    unsigned int phase,
                                    const Sequence &context) const {
  int index = pos;
  for (unsigned int j = 0; j < _models.size(); j++) {
    index -= _max_length[j];
    if (index < 0)
      return _models[j]->standardGenerator(generator->randomNumberGenerator())
                       ->drawSymbol(pos, phase, context);
  }
  return _models.back()->standardGenerator(generator->randomNumberGenerator())
                       ->drawSymbol(pos, phase, context);
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
