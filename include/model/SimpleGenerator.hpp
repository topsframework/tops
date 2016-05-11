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

#ifndef TOPS_MODEL_SIMPLE_GENERATOR_
#define TOPS_MODEL_SIMPLE_GENERATOR_

// Standard headers
#include <memory>
#include <typeinfo>
#include <exception>

// ToPS headers
#include "model/Generator.hpp"
#include "model/MemberDelegator.hpp"

namespace tops {
namespace model {

// Forward declaration
template<template<typename Target> class Decorator, typename Model>
class SimpleGenerator;

/**
 * @typedef SimpleGeneratorPtr
 * @brief Alias of pointer to SimpleGenerator.
 */
template<template<typename Target> class Decorator, typename Model>
using SimpleGeneratorPtr = std::shared_ptr<SimpleGenerator<Decorator, Model>>;

/**
 * @class SimpleGenerator
 * @brief TODO
 */
template<template<typename Target> class Decorator, typename Model>
class SimpleGenerator : public Generator<Decorator> {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;

  using Self = SimpleGenerator<Decorator, Model>;
  using SelfPtr = std::shared_ptr<Self>;

  // Static methods
  template<typename... Ts>
  static SimpleGeneratorPtr<Decorator, Model> make(Ts... args) {
    return std::shared_ptr<Self>(new Self(std::forward<Ts>(args)...));
  }

  // Overriden methods
  Decorator<Symbol> drawSymbol(unsigned int pos,
                               unsigned int phase,
                               const Sequence& context) const override {
    CALL_MEMBER_FUNCTION_DELEGATOR(drawSymbol, pos, phase, context);
  }

  Decorator<Sequence> drawSequence(unsigned int size,
                                   unsigned int phase) const override {
    CALL_MEMBER_FUNCTION_DELEGATOR(drawSequence, size, phase);
  }

  RandomNumberGeneratorPtr randomNumberGenerator() const override {
    return _generator;
  }

 protected:
  // Alias
  using RNG = RandomNumberGenerator;
  using RNGPtr = std::shared_ptr<RNG>;

  // Instace variables
  ModelPtr _model;
  RNGPtr _generator;

  // Constructors
  SimpleGenerator(ModelPtr m, RNGPtr generator)
      : _model(std::move(m)), _generator(std::move(generator)) {
  }

 private:
  GENERATE_MEMBER_FUNCTION_DELEGATOR(drawSymbol, _model)
  GENERATE_MEMBER_FUNCTION_DELEGATOR(drawSequence, _model)
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_SIMPLE_GENERATOR_
