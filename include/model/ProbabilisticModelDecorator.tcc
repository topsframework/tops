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
class ProbabilisticModelDecorator;

/**
 * @typedef ProbabilisticModelDecoratorPtr
 * @brief Alias of pointer to ProbabilisticModelDecorator.
 */
template<typename Derived>
using ProbabilisticModelDecoratorPtr
    = std::shared_ptr<ProbabilisticModelDecorator<Derived>>;

/**
 * @class ProbabilisticModelDecorator
 * @brief Base class that defines probabilistic models' decorators.
 *
 * It is the easiest way to change the behavior of a probabilistic model.
 */
template<typename Derived>
class ProbabilisticModelDecorator
    : public ProbabilisticModelCrtp<Derived> {
 public:
  // Alias
  using Base = ProbabilisticModelCrtp<Derived>;

  using Self = ProbabilisticModelDecorator<Derived>;
  using SelfPtr = std::shared_ptr<Self>;

  template<template<typename Target> class Decorator>
  using SGPtr = SimpleGeneratorPtr<Decorator, Derived>;

  // Static methods
  template<typename... Ts>
  static SelfPtr make(Ts... args) {
    return std::shared_ptr<Self>(new Self(std::forward<Ts>(args)...));
  }

  // Overriden methods
  Standard<Symbol> simpleChooseSymbol(SGPtr<Standard> generator,
                                      unsigned int pos,
                                      const Sequence &context,
                                      unsigned int phase) const override {
    return _model->standardGenerator()->chooseSymbol(pos, context, phase);
  }

  double evaluate(const Sequence &s,
                  unsigned int pos,
                  unsigned int phase = 0) const override {
    return _model->evaluate(s, pos, phase);
  }

 protected:
  // Instance variables
  ProbabilisticModelPtr _model;

  // Constructors
  explicit ProbabilisticModelDecorator(ProbabilisticModelPtr model)
    : _model(model) {
  }
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PROBABILISTIC_MODEL_DECORATOR_
