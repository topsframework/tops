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
#include "model/MultipleSequentialModel.hpp"

// Standard headers
#include <cmath>
#include <limits>
#include <vector>
#include <cstdlib>
#include <iostream>

// Internal headers
#include "model/Util.hpp"

#include "exception/NotYetImplemented.hpp"
#include "exception/InvalidModelDefinition.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

MultipleSequentialModel::MultipleSequentialModel(
    std::vector<ProbabilisticModelPtr> models,
    std::vector<int> max_length)
    : _models(models), _max_length(max_length) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

Probability
MultipleSequentialModel::evaluateSymbol(SEPtr<Standard> /* evaluator */,
                                        size_t /* pos */,
                                        size_t /* phase */) const {
  throw_exception(NotYetImplemented);
}

/*----------------------------------------------------------------------------*/

Probability
MultipleSequentialModel::evaluateSequence(SEPtr<Standard> evaluator,
                                          size_t begin,
                                          size_t end,
                                          size_t phase) const {
  if (begin > end) return 0;

  Probability product = 1;
  int b = begin;
  int e = 0;

  for (size_t i = 0; i < _models.size(); i++) {
    e = b + _max_length[i];
    if (e >= static_cast<int>(evaluator->sequence().size()))
      e = evaluator->sequence().size();
    product *= _models[i]->standardEvaluator(evaluator->sequence())
                         ->evaluateSequence(b, e, phase);
    if (e + 1 >= static_cast<int>(end))
      return product;
    phase = mod(phase + e - b + 1, _models.size());
    b = e;
  }

  return product;
}

/*----------------------------------------------------------------------------*/

void MultipleSequentialModel::initializeCache(CEPtr<Standard> evaluator,
                                              size_t /* phase */) {
  evaluator->cache().evaluators.resize(_models.size());

  for (size_t i = 0; i < _models.size(); i++)
    evaluator->cache().evaluators[i]
      = _models[i]->standardEvaluator(evaluator->sequence(), true);
}

/*----------------------------------------------------------------------------*/

Probability
MultipleSequentialModel::evaluateSymbol(CEPtr<Standard> evaluator,
                                        size_t pos,
                                        size_t phase) const {
  return Base::evaluateSymbol(evaluator, pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability
MultipleSequentialModel::evaluateSequence(CEPtr<Standard> evaluator,
                                          size_t begin,
                                          size_t end,
                                          size_t phase) const {
  auto& evaluators = evaluator->cache().evaluators;

  Probability product = 1;
  int b = begin;
  int e = 0;

  for (size_t i = 0; i < _models.size(); i++) {
    e = b + _max_length[i];
    if (e >= static_cast<int>(evaluator->sequence().size()))
      e = evaluator->sequence().size();
    product *= evaluators[i]->evaluateSequence(b, e, phase);
    if (e + 1 >= static_cast<int>(end))
      return product;
    phase = mod(phase + e - b + 1, _models.size());
    b = e;
  }

  return product;
}

/*===============================  GENERATOR  ================================*/

Standard<Symbol>
MultipleSequentialModel::drawSymbol(SGPtr<Standard> generator,
                                    size_t pos,
                                    size_t phase,
                                    const Sequence& context) const {
  int index = pos;
  for (size_t j = 0; j < _models.size(); j++) {
    index -= _max_length[j];
    if (index < 0)
      return _models[j]->standardGenerator(generator->randomNumberGenerator())
                       ->drawSymbol(pos, phase, context);
  }
  return _models.back()->standardGenerator(generator->randomNumberGenerator())
                       ->drawSymbol(pos, phase, context);
}

/*----------------------------------------------------------------------------*/

Standard<Sequence> MultipleSequentialModel::drawSequence(
    SGPtr<Standard> generator,
    size_t size,
    size_t phase) const {
  return Base::drawSequence(generator, size, phase);
}

/*===============================  SERIALIZER  ===============================*/

void MultipleSequentialModel::serialize(SSPtr serializer) {
  Base::serialize(serializer);
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
