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

// ToPS headers
#include "model/Sequence.hpp"

namespace tops {
namespace model {

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

template<typename Model>
class SimpleEvaluatorImpl;

/**
 * @typedef SimpleEvaluatorImplPtr
 * @brief Alias of pointer to SimpleEvaluatorImpl.
 */
template<typename Model>
using SimpleEvaluatorImplPtr = std::shared_ptr<SimpleEvaluatorImpl<Model>>;

/**
 * @class SimpleEvaluatorImpl
 * @brief TODO
 */
template<typename Model>
class SimpleEvaluatorImpl : public EvaluatorImpl {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;

  // Static methods
  template<typename... Ts>
  static SimpleEvaluatorImplPtr<Model> make(Ts... args);

  // Concrete methods
  virtual double probabilityOf(unsigned int begin,
                               unsigned int end,
                               unsigned int phase = 0) override;

  Labeling labeling(Labeling::Method method);

  Sequence& sequence() { return _sequence; }
  const Sequence& sequence() const { return _sequence; }

 protected:
  // Instace variables
  ModelPtr _model;
  Sequence _sequence;

  // Constructors
  SimpleEvaluatorImpl(ModelPtr m, const Sequence &s);
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
SimpleEvaluatorImplPtr<Model> SimpleEvaluatorImpl<Model>::make(Ts... args) {
  auto evaluator = SimpleEvaluatorImplPtr<Model>(
      new SimpleEvaluatorImpl<Model>(std::forward<Ts>(args)...));
  return evaluator;
}

/*----------------------------------------------------------------------------*/
/*                            Concrete methods                                */
/*----------------------------------------------------------------------------*/

template<typename Model>
double SimpleEvaluatorImpl<Model>::probabilityOf(unsigned int begin,
                                             unsigned int end,
                                             unsigned int phase) {
  return this->_model->probabilityOf(
    std::static_pointer_cast<SimpleEvaluatorImpl<Model>>(
      this->shared_from_this()),
    begin,
    end,
    phase);
}

/*----------------------------------------------------------------------------*/
/*                              Constructors                                  */
/*----------------------------------------------------------------------------*/

template<typename Model>
SimpleEvaluatorImpl<Model>::SimpleEvaluatorImpl(ModelPtr m, const Sequence &s)
    : _model(m), _sequence(s) {
}

}  // namespace model
}  // namespace tops

#endif
