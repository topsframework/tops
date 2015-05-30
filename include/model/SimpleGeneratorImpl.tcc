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

#ifndef TOPS_MODEL_SIMPLE_GENERATOR_IMPL_
#define TOPS_MODEL_SIMPLE_GENERATOR_IMPL_

// Standard headers
#include <memory>
#include <exception>
#include <type_traits>

// ToPS headers
#include "model/Sequence.hpp"
#include "model/MemberDetector.hpp"

// ToPS templates
#include "model/Identity.tcc"
#include "model/Labeling.tcc"
#include "model/Estimation.tcc"

namespace tops {
namespace model {

// Forward declaration
class DecodableModel;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

template<typename Model>
class SimpleGeneratorImpl;

// Auxiliar tests
GENERATE_HAS_MEMBER(chooseSequence)
GENERATE_HAS_MEMBER(chooseLabeling)

/**
 * @typedef SimpleGeneratorImplPtr
 * @brief Alias of pointer to SimpleGeneratorImpl.
 */
template<typename Model>
using SimpleGeneratorImplPtr
    = std::shared_ptr<SimpleGeneratorImpl<Model>>;

/**
 * @class SimpleGeneratorImpl
 * @brief TODO
 */
template<typename Model>
class SimpleGeneratorImpl : public GeneratorImpl {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;

  // Static methods
  template<typename... Ts>
  static SimpleGeneratorImplPtr<Model> make(Ts... args);

  // Virtual methods
  virtual Identity<Sequence> chooseSequence(unsigned int size,
                                            unsigned int phase);
  virtual Labeling<Sequence> chooseLabeling(unsigned int size,
                                            unsigned int phase);

 protected:
  // Instace variables
  ModelPtr _model;

  // Constructors
  SimpleGeneratorImpl(ModelPtr m);

 private:
  // Concrete methods
  template<typename M = Model>
  Identity<Sequence> chooseSequenceImpl(unsigned int size,
                                        unsigned int phase,
                                        no_chooseSequence<M>* dummy = nullptr);

  template<typename M = Model>
  Identity<Sequence> chooseSequenceImpl(unsigned int size,
                                        unsigned int phase,
                                        has_chooseSequence<M>* dummy = nullptr);

  template<typename M = Model>
  Labeling<Sequence> chooseLabelingImpl(unsigned int size,
                                        unsigned int phase,
                                        no_chooseLabeling<M>* dummy = nullptr);

  template<typename M = Model>
  Labeling<Sequence> chooseLabelingImpl(unsigned int size,
                                        unsigned int phase,
                                        has_chooseLabeling<M>* dummy = nullptr);
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                               IMPLEMENTATION
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/*----------------------------------------------------------------------------*/
/*                             Static methods                                 */
/*----------------------------------------------------------------------------*/

template<typename Model>
template<typename... Ts>
SimpleGeneratorImplPtr<Model>
SimpleGeneratorImpl<Model>::make(Ts... args) {
  auto generator = SimpleGeneratorImplPtr<Model>(
      new SimpleGeneratorImpl<Model>(std::forward<Ts>(args)...));
  return generator;
}

/*----------------------------------------------------------------------------*/
/*                             Virtual methods                                */
/*----------------------------------------------------------------------------*/

template<typename Model>
Identity<Sequence>
SimpleGeneratorImpl<Model>::chooseSequence(unsigned int size,
                                           unsigned int phase) {
  return chooseSequenceImpl(size, phase);
}

template<typename Model>
Labeling<Sequence>
SimpleGeneratorImpl<Model>::chooseLabeling(unsigned int size,
                                           unsigned int phase) {
  return chooseLabelingImpl(size, phase);
}

/*----------------------------------------------------------------------------*/
/*                            Concrete methods                                */
/*----------------------------------------------------------------------------*/

template<typename Model>
template<typename M>
Identity<Sequence>
SimpleGeneratorImpl<Model>::chooseSequenceImpl(unsigned int size,
                                               unsigned int phase,
                                               no_chooseSequence<M>* dummy) {
  throw std::logic_error("Model does not have method chooseSequence");
}

template<typename Model>
template<typename M>
Identity<Sequence>
SimpleGeneratorImpl<Model>::chooseSequenceImpl(unsigned int size,
                                               unsigned int phase,
                                               has_chooseSequence<M>* dummy) {
  return this->_model->chooseSequence(
    std::static_pointer_cast<SimpleGeneratorImpl<M>>(
      this->shared_from_this()),
      size, phase);
}

template<typename Model>
template<typename M>
Labeling<Sequence>
SimpleGeneratorImpl<Model>::chooseLabelingImpl(unsigned int size,
                                               unsigned int phase,
                                               no_chooseLabeling<M>* dummy) {
  throw std::logic_error("Model does not have method chooseLabeling");
}

template<typename Model>
template<typename M>
Labeling<Sequence>
SimpleGeneratorImpl<Model>::chooseLabelingImpl(unsigned int size,
                                               unsigned int phase,
                                               has_chooseLabeling<M>* dummy) {
  return this->_model->chooseLabeling(
    std::static_pointer_cast<SimpleGeneratorImpl<M>>(
      this->shared_from_this()),
      size, phase);
}

/*----------------------------------------------------------------------------*/
/*                              Constructors                                  */
/*----------------------------------------------------------------------------*/

template<typename Model>
SimpleGeneratorImpl<Model>::SimpleGeneratorImpl(ModelPtr m)
    : _model(m) {
}

}  // namespace model
}  // namespace tops

#endif
