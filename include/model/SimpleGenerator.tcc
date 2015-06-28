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

// ToPS templates
#include "model/Generator.tcc"
#include "model/MemberDelegator.tcc"

namespace tops {
namespace model {

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

template<template<typename Target> class Decorator,
         typename Model, bool is_base>
class SimpleGenerator;

/**
 * @typedef SimpleGeneratorPtr
 * @brief Alias of pointer to SimpleGenerator.
 */
template<template<typename Target> class Decorator,
         typename Model, bool is_base = false>
using SimpleGeneratorPtr
    = std::shared_ptr<SimpleGenerator<Decorator, Model, is_base>>;

/**
 * @class SimpleGenerator
 * @brief TODO
 */
template<template<typename Target> class Decorator,
         typename Model, bool is_base = false>
class SimpleGenerator
    : public std::conditional<!std::is_void<typename Model::Base>::value,
               SimpleGenerator<Decorator, typename Model::Base, true>,
               Generator<Decorator>>::type {
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
                               const Sequence &context) const override {
    CALL_METHOD_DELEGATOR(drawSymbol, _model, pos, phase, context);
  }

  Decorator<Sequence> drawSequence(unsigned int size,
                                   unsigned int phase) const override {
    CALL_METHOD_DELEGATOR(drawSequence, _model, size, phase);
  }

 protected:
  // Instace variables
  ModelPtr _model;

  // Constructors
  SimpleGenerator(ModelPtr m = ModelPtr())
      : _model(std::move(m)) {
  }

 private:
  GENERATE_METHOD_DELEGATOR(drawSymbol, _model)
  GENERATE_METHOD_DELEGATOR(drawSequence, _model)
};

}  // namespace model
}  // namespace tops

#endif
