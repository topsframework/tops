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

#ifndef TOPS_MODEL_CACHED_EVALUATOR_MODEL_
#define TOPS_MODEL_CACHED_EVALUATOR_MODEL_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Sequence.hpp"

// ToPS templates
#include "model/SimpleEvaluator.tcc"

namespace tops {
namespace model {

template<typename Model>
class CachedEvaluator;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                     CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef CachedEvaluatorPtr
 * @brief Alias of pointer to CachedEvaluator.
 */
template<typename Model>
using CachedEvaluatorPtr = std::shared_ptr<CachedEvaluator<Model>>;

/**
 * @class CachedEvaluator
 * @brief TODO
 */
template<typename Model>
class CachedEvaluator : public SimpleEvaluator<Model> {
 public:
  // Alias
  using cache = typename Model::cache;
  using ModelPtr = std::shared_ptr<Model>;

  // Static methods
  template<typename... Ts>
  static CachedEvaluatorPtr<Model> make(Ts... args);

  // Concrete methods
  virtual double probabilityOf(unsigned int begin,
                               unsigned int end,
                               unsigned int phase = 0) override;

  cache& memory() {
    return _memory;
  }

 private:
  // Instance variables
  cache _memory;

  // Constructors
  CachedEvaluator(ModelPtr m, const Sequence &s,
                  cache&& memory);
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
CachedEvaluatorPtr<Model> CachedEvaluator<Model>::make(Ts... args) {
  auto evaluator = CachedEvaluatorPtr<Model>(
      new CachedEvaluator<Model>(std::forward<Ts>(args)...));
  return evaluator;
}

/*----------------------------------------------------------------------------*/
/*                            Concrete methods                                */
/*----------------------------------------------------------------------------*/

template<typename Model>
double CachedEvaluator<Model>::probabilityOf(unsigned int begin,
                                             unsigned int end,
                                             unsigned int phase) {
  this->_model->initializePrefixSumArray(
      std::static_pointer_cast<CachedEvaluator<Model>>(
      this->shared_from_this()));
  return this->_model->evaluateWithPrefixSumArray(
      std::static_pointer_cast<CachedEvaluator<Model>>(
      this->shared_from_this()), begin, end, phase);
}

/*----------------------------------------------------------------------------*/
/*                              Constructors                                  */
/*----------------------------------------------------------------------------*/

template<typename Model>
CachedEvaluator<Model>::CachedEvaluator(ModelPtr m,
                                        const Sequence &s,
                                        cache&& memory)
    : SimpleEvaluator<Model>(m, s), _memory(std::forward<cache>(memory)) {
}

}  // namespace model
}  // namespace tops

#endif
