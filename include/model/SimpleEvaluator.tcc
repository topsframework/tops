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
class SimpleEvaluator;

/**
 * @typedef SimpleEvaluatorPtr
 * @brief Alias of pointer to SimpleEvaluator.
 */
template<typename Model>
using SimpleEvaluatorPtr = std::shared_ptr<SimpleEvaluator<Model>>;

/**
 * @class SimpleEvaluator
 * @brief TODO
 */
template<typename Model>
class SimpleEvaluator : public Evaluator {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;

  // Static methods
  template<typename... Ts>
  static SimpleEvaluatorPtr<Model> make(Ts... args);

  // Concrete methods
  virtual double probabilityOf(unsigned int begin,
                               unsigned int end,
                               unsigned int phase = 0) override;
  Sequence sequence;

 protected:
  // Instace variables
  ModelPtr _model;

  // Constructors
  SimpleEvaluator(ModelPtr m, const Sequence &s);
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
SimpleEvaluatorPtr<Model> SimpleEvaluator<Model>::make(Ts... args) {
  auto evaluator = SimpleEvaluatorPtr<Model>(
      new SimpleEvaluator<Model>(std::forward<Ts>(args)...));
  return evaluator;
}

/*----------------------------------------------------------------------------*/
/*                            Concrete methods                                */
/*----------------------------------------------------------------------------*/

template<typename Model>
double SimpleEvaluator<Model>::probabilityOf(unsigned int begin,
                                             unsigned int end,
                                             unsigned int phase) {
  return _model->evaluateSequence(sequence, begin, end, phase);
}

/*----------------------------------------------------------------------------*/
/*                              Constructors                                  */
/*----------------------------------------------------------------------------*/

template<typename Model>
SimpleEvaluator<Model>::SimpleEvaluator(ModelPtr m, const Sequence &s)
    : sequence(s), _model(m) {
}

}  // namespace model
}  // namespace tops

#endif
