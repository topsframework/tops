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
#include "model/FixedSequenceAtPosition.hpp"

// Standard headers
#include <vector>

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

FixedSequenceAtPosition::FixedSequenceAtPosition(ProbabilisticModelPtr model,
                                                 int position,
                                                 Sequence sequence,
                                                 DiscreteIIDModelPtr distr)
      : _model(model), _position(position), _sequence(sequence),
        _probabilities(distr) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*==============================  EVALUATOR  =================================*/

Probability FixedSequenceAtPosition::evaluateSymbol(
    SEPtr<Multiple> evaluator,
    size_t pos,
    size_t phase) const {
  auto modelEvaluator = _model->standardEvaluator(evaluator->sequence());
  return modelEvaluator->evaluateSymbol(pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability FixedSequenceAtPosition::evaluateSequence(
    SEPtr<Multiple> evaluator,
    size_t begin,
    size_t end,
    size_t phase) const {
  auto modelEvaluator = _model->standardEvaluator(evaluator->sequence());
  auto result = modelEvaluator->evaluateSequence(begin, end, phase);

  int j;
  for (j = 0;
       (j < static_cast<int>(_sequence.size()))
       && ((_position  + j) < static_cast<int>(evaluator->sequence().size()));
       j++) {
    if (_sequence[j] != evaluator->sequence()[0][_position + j] )
      break;
  }
  if (j != static_cast<int>(_sequence.size()))
    result *= _probabilities->probabilityOf(1);
  else
    result *= _probabilities->probabilityOf(0);
  return result;
}

/*----------------------------------------------------------------------------*/

void FixedSequenceAtPosition::initializeCache(CEPtr<Multiple> evaluator,
                                              size_t phase) {
  Base::initializeCache(evaluator, phase);
}

/*----------------------------------------------------------------------------*/

Probability FixedSequenceAtPosition::evaluateSymbol(
    CEPtr<Multiple> evaluator,
    size_t pos,
    size_t phase) const {
  return Base::evaluateSymbol(evaluator, pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability FixedSequenceAtPosition::evaluateSequence(
    CEPtr<Multiple> evaluator,
    size_t begin,
    size_t end,
    size_t phase) const {
  return Base::evaluateSequence(evaluator, begin, end, phase);
}

/*==============================  GENERATOR  =================================*/

Multiple<Symbol> FixedSequenceAtPosition::drawSymbol(
    SGPtr<Multiple> generator,
    size_t pos,
    size_t phase,
    const Multiple<Sequence> &context) const {
  auto modelGenerator
    = _model->standardGenerator(generator->randomNumberGenerator());
  return modelGenerator->drawSymbol(pos, phase, context);
}

/*----------------------------------------------------------------------------*/

Multiple<Sequence> FixedSequenceAtPosition::drawSequence(
    SGPtr<Multiple> generator,
    size_t size,
    size_t phase) const {
  auto model_generator
    = _model->standardGenerator(generator->randomNumberGenerator());

  auto sequences = model_generator->drawSequence(size, phase);

  addSequence(sequences[0]);
  return sequences;
}

/*===============================  SERIALIZER  ===============================*/

void FixedSequenceAtPosition::serialize(SSPtr serializer) {
  Base::serialize(serializer);
}

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

void FixedSequenceAtPosition::addSequence(Sequence& h) const {
  auto rng = _probabilities->standardGenerator()->randomNumberGenerator();
  if (_probabilities->draw(rng) == 1)
    return;
  for (int i = _position;
       ((i-_position) < static_cast<int>(_sequence.size()))
       && (i < static_cast<int>(h.size()));
       i++) {
    h[i] = _sequence[i-_position];
  }
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
