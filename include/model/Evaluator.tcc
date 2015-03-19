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

#ifndef TOPS_MODEL_EVALUATOR_MODEL_
#define TOPS_MODEL_EVALUATOR_MODEL_

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
class Evaluator;

/**
 * @typedef EvaluatorPtr
 * @brief Alias of pointer to Evaluator.
 */
template<typename Model>
using EvaluatorPtr = std::shared_ptr<Evaluator<Model>>;

/**
 * @class Evaluator
 * @brief TODO
 */
template<typename Model>
class Evaluator : public std::enable_shared_from_this<Evaluator<Model>> {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;

  // Static methods
  template<typename... Ts>
  static EvaluatorPtr<Model> make(Ts... args);

  // Concrete methods
  virtual double probabilityOf(unsigned int begin,
                               unsigned int end,
                               unsigned int phase = 0);
  Sequence sequence;

 protected:
  // Instace variables
  ModelPtr _model;

  // Constructors
  Evaluator(ModelPtr m, const Sequence &s);
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
EvaluatorPtr<Model> Evaluator<Model>::make(Ts... args) {
  auto evaluator = EvaluatorPtr<Model>(
      new Evaluator<Model>(std::forward<Ts>(args)...));
  return evaluator;
}

/*----------------------------------------------------------------------------*/
/*                            Concrete methods                                */
/*----------------------------------------------------------------------------*/

template<typename Model>
double Evaluator<Model>::probabilityOf(unsigned int begin,
                                       unsigned int end,
                                       unsigned int phase) {
  return _model->evaluateSequence(sequence, begin, end, phase);
}

/*----------------------------------------------------------------------------*/
/*                              Constructors                                  */
/*----------------------------------------------------------------------------*/

template<typename Model>
Evaluator<Model>::Evaluator(ModelPtr m, const Sequence &s)
    : sequence(s), _model(m) {
}

}  // namespace model
}  // namespace tops

#endif
