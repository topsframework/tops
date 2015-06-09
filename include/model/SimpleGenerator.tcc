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
#include "model/MemberDetector.hpp"

// ToPS templates
#include "model/Generator.tcc"

namespace tops {
namespace model {

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

// Auxiliar tests
GENERATE_HAS_MEMBER(simpleChooseSequence)

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
  Decorator<Sequence> chooseSequence(unsigned int size,
                                     unsigned int phase) override {
    return chooseSequence(size, phase,
      typename has_member_simpleChooseSequence<
        Model, const Decorator<Sequence>(SelfPtr, unsigned int, unsigned int)
      >::tag());
  }

 protected:
  // Instace variables
  ModelPtr _model;

  // Constructors
  SimpleGenerator(ModelPtr m = ModelPtr())
      : _model(std::move(m)) {
  }

 private:
  // Concrete methods
  Decorator<Sequence> chooseSequence(unsigned int size,
                                     unsigned int phase,
                                     no_simpleChooseSequence_tag) {
    static_assert(is_base,
      "Model does not have method simpleChooseSequence");
    throw std::logic_error(
      "Calling from base with no method simpleChooseSequence");
  }

  Decorator<Sequence> chooseSequence(unsigned int size,
                                     unsigned int phase,
                                     has_simpleChooseSequence_tag) {
    return _model->simpleChooseSequence(make_shared(), size, phase);
  }

  SimpleGeneratorPtr<Decorator, Model> make_shared() {
    return std::static_pointer_cast<Self>(this->shared_from_this());
  }
};

}  // namespace model
}  // namespace tops

#endif
