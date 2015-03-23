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

#ifndef TOPS_MODEL_CACHED_EVALUATOR_IMPL_
#define TOPS_MODEL_CACHED_EVALUATOR_IMPL_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Sequence.hpp"

// ToPS templates
#include "model/SimpleEvaluatorImpl.tcc"

namespace tops {
namespace model {

template<typename Model>
class CachedEvaluatorImpl;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                     CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef CachedEvaluatorImplPtr
 * @brief Alias of pointer to CachedEvaluatorImpl.
 */
template<typename Model>
using CachedEvaluatorImplPtr = std::shared_ptr<CachedEvaluatorImpl<Model>>;

/**
 * @class CachedEvaluatorImpl
 * @brief TODO
 */
template<typename Model>
class CachedEvaluatorImpl : public SimpleEvaluatorImpl<Model> {
 public:
  // Alias
  using cache = typename Model::cache;
  using ModelPtr = std::shared_ptr<Model>;

  // Static methods
  template<typename... Ts>
  static CachedEvaluatorImplPtr<Model> make(Ts... args);

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
  CachedEvaluatorImpl(ModelPtr m, const Sequence &s,
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
CachedEvaluatorImplPtr<Model> CachedEvaluatorImpl<Model>::make(Ts... args) {
  auto evaluator = CachedEvaluatorImplPtr<Model>(
      new CachedEvaluatorImpl<Model>(std::forward<Ts>(args)...));
  return evaluator;
}

/*----------------------------------------------------------------------------*/
/*                            Concrete methods                                */
/*----------------------------------------------------------------------------*/

template<typename Model>
double CachedEvaluatorImpl<Model>::probabilityOf(unsigned int begin,
                                             unsigned int end,
                                             unsigned int phase) {
  this->_model->initializeCachedEvaluator(
      std::static_pointer_cast<CachedEvaluatorImpl<Model>>(
      this->shared_from_this()));
  return this->_model->cachedProbabilityOf(
      std::static_pointer_cast<CachedEvaluatorImpl<Model>>(
      this->shared_from_this()), begin, end, phase);
}

/*----------------------------------------------------------------------------*/
/*                              Constructors                                  */
/*----------------------------------------------------------------------------*/

template<typename Model>
CachedEvaluatorImpl<Model>::CachedEvaluatorImpl(ModelPtr m,
                                        const Sequence &s,
                                        cache&& memory)
    : SimpleEvaluatorImpl<Model>(m, s), _memory(std::forward<cache>(memory)) {
}

}  // namespace model
}  // namespace tops

#endif
