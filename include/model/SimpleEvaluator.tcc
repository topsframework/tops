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

#ifndef TOPS_MODEL_SIMPLE_EVALUATOR_
#define TOPS_MODEL_SIMPLE_EVALUATOR_

// Standard headers
#include <memory>
#include <type_traits>

// ToPS headers
#include "model/Sequence.hpp"

// ToPS templates
#include "model/Labeling.tcc"
#include "model/Estimation.tcc"

namespace tops {
namespace model {

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

template<template<typename Target> class Decorator, typename Model>
class SimpleEvaluator;

/**
 * @typedef SimpleEvaluatorPtr
 * @brief Alias of pointer to SimpleEvaluator.
 */
template<template<typename Target> class Decorator, typename Model>
using SimpleEvaluatorPtr = std::shared_ptr<SimpleEvaluator<Decorator, Model>>;

/**
 * @class SimpleEvaluator
 * @brief TODO
 */
template<template<typename Target> class Decorator, typename Model>
class SimpleEvaluator : public Evaluator<Decorator> {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;

  using Self = SimpleEvaluator<Decorator, Model>;
  using SelfPtr = std::shared_ptr<Self>;

  // Static methods
  template<typename... Ts>
  static SimpleEvaluatorPtr<Decorator, Model> make(Ts... args) {
    return std::shared_ptr<Self>(new Self(std::forward<Ts>(args)...));
  }

  // Overriden methods
  Probability evaluateSymbol(unsigned int pos,
                             unsigned int phase) const override {
    CALL_METHOD_DELEGATOR(evaluateSymbol, pos, phase);
  }

  Probability evaluateSequence(unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override {
    CALL_METHOD_DELEGATOR(evaluateSequence, begin, end, phase);
  }

  Decorator<Sequence>& sequence() override {
    return _sequence;
  }

  const Decorator<Sequence>& sequence() const override {
    return _sequence;
  }

 protected:
  // Instace variables
  ModelPtr _model;
  Decorator<Sequence> _sequence;

  // Constructors
  SimpleEvaluator(ModelPtr model, Decorator<Sequence> sequence)
      : _model(std::move(model)), _sequence(std::move(sequence)) {
  }

 private:
  GENERATE_METHOD_DELEGATOR(evaluateSymbol, _model)
  GENERATE_METHOD_DELEGATOR(evaluateSequence, _model)
};

}  // namespace model
}  // namespace tops

#endif
