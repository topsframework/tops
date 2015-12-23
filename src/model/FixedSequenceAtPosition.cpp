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
#include <vector>

// ToPS headers
#include "model/FixedSequenceAtPosition.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                             CONCRETE METHODS                               */
/*----------------------------------------------------------------------------*/

/*==============================  EVALUATOR  =================================*/

Probability
FixedSequenceAtPosition::evaluateSequence(SEPtr<Standard> evaluator,
                                          unsigned int begin,
                                          unsigned int end,
                                          unsigned int phase) const {
  auto modelEvaluator = _model->standardEvaluator(evaluator->sequence());
  auto result = modelEvaluator->evaluateSequence(begin, end, phase);

  int j;
  for (j = 0;
       (j < static_cast<int>(_sequence.size()))
       && ((_position  + j) < static_cast<int>(evaluator->sequence().size()));
       j++) {
    if (_sequence[j] != evaluator->sequence()[_position + j] )
      break;
  }
  if (j != static_cast<int>(_sequence.size()))
    result += _probabilities->probabilityOf(1);
  else
    result += _probabilities->probabilityOf(0);
  return result;
}

/*==============================  GENERATOR  =================================*/

Standard<Sequence>
FixedSequenceAtPosition::drawSequence(SGPtr<Standard> generator,
                                      unsigned int size,
                                      unsigned int phase) const {
  auto model_generator
    = _model->standardGenerator(generator->randomNumberGenerator());
  auto sequence = model_generator->drawSequence(size, phase);

  addSequence(sequence);
  return sequence;
}

/*================================  OTHERS  ==================================*/

void FixedSequenceAtPosition::addSequence(Sequence &h) const {
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
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

FixedSequenceAtPosition::FixedSequenceAtPosition(ProbabilisticModelPtr model,
                                                 int position,
                                                 Sequence sequence,
                                                 DiscreteIIDModelPtr distr)
      : ProbabilisticModelDecoratorCrtp(model),
        _position(position),
        _sequence(sequence),
        _probabilities(distr) {
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
