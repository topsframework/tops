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

#ifndef TOPS_MODEL_SIMPLE_EVALUATOR_IMPL_
#define TOPS_MODEL_SIMPLE_EVALUATOR_IMPL_

// Standard headers
#include <memory>
#include <type_traits>

// ToPS headers
#include "model/Sequence.hpp"
#include "model/Labeling.hpp"

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

template<typename Model, typename Decodable>
class SimpleEvaluatorImpl;

// Auxiliar tests
template<typename Model>
using is_decodable
    = std::enable_if_t<std::is_base_of<DecodableModel, Model>::value>;

/**
 * @typedef SimpleEvaluatorImplPtr
 * @brief Alias of pointer to SimpleEvaluatorImpl.
 */
template<typename Model, typename Decodable = void>
using SimpleEvaluatorImplPtr
    = std::shared_ptr<SimpleEvaluatorImpl<Model, Decodable>>;

/**
 * @class SimpleEvaluatorImpl
 * @brief TODO
 */
template<typename Model, typename Decodable = void>
class SimpleEvaluatorImpl : public EvaluatorImpl {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;

  // Static methods
  template<typename... Ts>
  static SimpleEvaluatorImplPtr<Model, Decodable> make(Ts... args);

  // Virtual methods
  virtual double probabilityOf(unsigned int begin,
                               unsigned int end,
                               unsigned int phase = 0) override;

  virtual Labeling labeling(Labeling::Method method) override;

  // Virtual getters
  virtual Sequence& sequence() override { return _sequence; }
  virtual const Sequence& sequence() const override { return _sequence; }

 protected:
  // Instace variables
  ModelPtr _model;
  Sequence _sequence;

  // Constructors
  SimpleEvaluatorImpl(ModelPtr m, const Sequence &s);
};

template<typename Model>
class SimpleEvaluatorImpl<Model, is_decodable<Model>> : public EvaluatorImpl {
 public:
  // Virtual methods
  virtual Labeling labeling(Labeling::Method method) override;

  // Virtual getters
  virtual Sequence& sequence() override { return _sequence; }
  virtual const Sequence& sequence() const override { return _sequence; }

 protected:
  // Instace variables
  Sequence _sequence;
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

template<typename Model, typename Decodable>
template<typename... Ts>
SimpleEvaluatorImplPtr<Model, Decodable>
SimpleEvaluatorImpl<Model, Decodable>::make(Ts... args) {
  auto evaluator = SimpleEvaluatorImplPtr<Model, Decodable>(
      new SimpleEvaluatorImpl<Model, Decodable>(std::forward<Ts>(args)...));
  return evaluator;
}

/*----------------------------------------------------------------------------*/
/*                            Concrete methods                                */
/*----------------------------------------------------------------------------*/

template<typename Model, typename Decodable>
double SimpleEvaluatorImpl<Model, Decodable>::probabilityOf(
    unsigned int begin,
    unsigned int end,
    unsigned int phase) {
  return this->_model->probabilityOf(
    std::static_pointer_cast<SimpleEvaluatorImpl<Model, Decodable>>(
      this->shared_from_this()),
    begin,
    end,
    phase);
}

template<typename Model, typename Decodable>
Labeling SimpleEvaluatorImpl<Model, Decodable>::labeling(
    Labeling::Method method) {
  return Labeling();
}

template<typename Model>
Labeling SimpleEvaluatorImpl<
    Model, is_decodable<Model>
>::labeling(Labeling::Method method) {
  return this->_model->simpleLabeling(
    std::static_pointer_cast<SimpleEvaluatorImpl<Model, is_decodable<Model>>>(
      this->shared_from_this()),
      method);
}

/*----------------------------------------------------------------------------*/
/*                              Constructors                                  */
/*----------------------------------------------------------------------------*/

template<typename Model, typename Decodable>
SimpleEvaluatorImpl<Model, Decodable>::SimpleEvaluatorImpl(ModelPtr m,
                                                           const Sequence &s)
    : _model(m), _sequence(s) {
}

}  // namespace model
}  // namespace tops

#endif
