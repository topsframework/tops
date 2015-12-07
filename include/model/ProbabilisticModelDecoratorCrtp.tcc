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

#ifndef TOPS_MODEL_PROBABILISTIC_MODEL_DECORATOR_
#define TOPS_MODEL_PROBABILISTIC_MODEL_DECORATOR_

// Standard headers
#include <memory>

// ToPS templates
#include "model/ProbabilisticModelCrtp.tcc"

namespace tops {
namespace model {

template<typename Derived>
class ProbabilisticModelDecoratorCrtp;

/**
 * @typedef ProbabilisticModelDecoratorCrtpPtr
 * @brief Alias of pointer to ProbabilisticModelDecoratorCrtp.
 */
template<typename Derived>
using ProbabilisticModelDecoratorCrtpPtr
    = std::shared_ptr<ProbabilisticModelDecoratorCrtp<Derived>>;

/**
 * @class ProbabilisticModelDecoratorCrtp
 * @brief Base class that defines probabilistic models' decorators.
 *
 * It is the easiest way to change the behavior of a probabilistic model.
 */
template<typename Derived>
class ProbabilisticModelDecoratorCrtp
    : public ProbabilisticModelCrtp<Derived> {
 public:
  // Alias
  using Base = ProbabilisticModelCrtp<Derived>;

  using Self = ProbabilisticModelDecoratorCrtp<Derived>;
  using SelfPtr = std::shared_ptr<Self>;

  template<template<typename Target> class Decorator>
  using SEPtr = SimpleEvaluatorPtr<Decorator, Derived>;

  template<template<typename Target> class Decorator>
  using CEPtr = CachedEvaluatorPtr<Decorator, Derived>;

  template<template<typename Target> class Decorator>
  using SGPtr = SimpleGeneratorPtr<Decorator, Derived>;

  // Constructors
  explicit ProbabilisticModelDecoratorCrtp(ProbabilisticModelPtr model)
    : _model(model) {
  }

  // Overriden methods
  Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                              unsigned int pos,
                              unsigned int phase,
                              const Sequence &context) const override {
    auto modelGenerator
      = _model->standardGenerator(generator->randomNumberGenerator());
    return modelGenerator->drawSymbol(pos, phase, context);
  }

  Probability evaluateSymbol(SEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override {
    auto modelEvaluator = _model->standardEvaluator(evaluator->sequence());
    return modelEvaluator->evaluateSymbol(pos, phase);
  }

 protected:
  // Instance variables
  ProbabilisticModelPtr _model;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PROBABILISTIC_MODEL_DECORATOR_
