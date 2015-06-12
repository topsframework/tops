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
/*                              STATIC METHODS                                */
/*----------------------------------------------------------------------------*/

FixedSequenceAtPositionPtr FixedSequenceAtPosition::make(
    ProbabilisticModelPtr model,
    int position,
    Sequence sequence,
    DiscreteIIDModelPtr distr) {
  return FixedSequenceAtPositionPtr(
    new FixedSequenceAtPosition(model, position, sequence, distr));
}

/*----------------------------------------------------------------------------*/
/*                             VIRTUAL METHODS                                */
/*----------------------------------------------------------------------------*/

/*==============================  EVALUATOR  =================================*/

EvaluatorPtr FixedSequenceAtPosition::evaluator(
    const Sequence &s,
    bool cached) {
  return Evaluator::make(
    SimpleEvaluatorImpl<FixedSequenceAtPosition>::make(
      std::static_pointer_cast<FixedSequenceAtPosition>(shared_from_this()),
      s));
}

/*----------------------------------------------------------------------------*/
/*                             CONCRETE METHODS                               */
/*----------------------------------------------------------------------------*/

/*==============================  EVALUATOR  =================================*/

double FixedSequenceAtPosition::simpleProbabilityOf(
    SEPtr evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int phase) const {
  auto result = _model->evaluator(evaluator->sequence())->probabilityOf(begin, end, phase);
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
FixedSequenceAtPosition::simpleChooseSequence(SGPtr<Standard> generator,
                                              unsigned int size,
                                              unsigned int phase) const {
  Sequence s = _model->standardGenerator()->chooseSequence(size, phase);
  addSequence(s);
  return s;
}

/*================================  OTHERS  ==================================*/

void FixedSequenceAtPosition::addSequence(Sequence &h) const {
  if (_probabilities->choose() == 1)
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
      : ProbabilisticModelDecorator(model),
        _position(position),
        _sequence(sequence),
        _probabilities(distr) {
}

}  // namespace model
}  // namespace tops
