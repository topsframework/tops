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

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename Derived>
ProbabilisticModelDecoratorCrtp<Derived>::ProbabilisticModelDecoratorCrtp(
    ProbabilisticModelPtr model)
    : _model(model) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

template<typename Derived>
Probability ProbabilisticModelDecoratorCrtp<Derived>::evaluateSymbol(
    SEPtr<Standard> evaluator,
    unsigned int pos,
    unsigned int phase) const {
  auto modelEvaluator = _model->standardEvaluator(evaluator->sequence());
  return modelEvaluator->evaluateSymbol(pos, phase);
}

/*===============================  GENERATOR  ================================*/

template<typename Derived>
Standard<Symbol> ProbabilisticModelDecoratorCrtp<Derived>::drawSymbol(
    SGPtr<Standard> generator,
    unsigned int pos,
    unsigned int phase,
    const Sequence& context) const {
  auto modelGenerator
    = _model->standardGenerator(generator->randomNumberGenerator());
  return modelGenerator->drawSymbol(pos, phase, context);
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
