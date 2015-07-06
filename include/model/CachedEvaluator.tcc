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
#include <type_traits>

// ToPS headers
#include "model/Sequence.hpp"

// ToPS templates
#include "model/Labeling.tcc"
#include "model/Estimation.tcc"

namespace tops {
namespace model {

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

template<template<typename Target> class Decorator, typename Model>
class CachedEvaluator;

/**
 * @typedef CachedEvaluatorPtr
 * @brief Alias of pointer to CachedEvaluator.
 */
template<template<typename Target> class Decorator, typename Model>
using CachedEvaluatorPtr = std::shared_ptr<CachedEvaluator<Decorator, Model>>;

/**
 * @class CachedEvaluator
 * @brief TODO
 */
template<template<typename Target> class Decorator, typename Model>
class CachedEvaluator : public SimpleEvaluator<Decorator, Model> {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;
  using Cache = typename Model::Cache;

  using Base = SimpleEvaluator<Decorator, Model>;
  using Self = CachedEvaluator<Decorator, Model>;
  using SelfPtr = std::shared_ptr<Self>;

  // Static methods
  template<typename... Ts>
  static CachedEvaluatorPtr<Decorator, Model> make(Ts... args) {
    return std::shared_ptr<Self>(new Self(std::forward<Ts>(args)...));
  }

  // Overriden methods
  Probability evaluateSymbol(unsigned int pos,
                             unsigned int phase) const override {
    const_cast<Self *>(this)->lazyInitializeCache(phase);
    CALL_METHOD_DELEGATOR(evaluateSymbol, pos, phase);
  }

  Probability evaluateSequence(unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override {
    const_cast<Self *>(this)->lazyInitializeCache(phase);
    CALL_METHOD_DELEGATOR(evaluateSequence, begin, end, phase);
  }

  // Virtual methods
  virtual void initializeCache(unsigned int phase) {
    CALL_METHOD_DELEGATOR(initializeCache, phase);
  }

  // Concrete methods
  Cache& cache() {
    return _cache;
  }

  const Cache& cache() const {
    return _cache;
  }

 protected:
  // Instace variables
  Cache _cache;
  bool initialized = false;

  // Constructors
  CachedEvaluator(
      ModelPtr model, Decorator<Sequence> sequence, Cache cache = Cache())
      : Base(std::move(model), std::move(sequence)), _cache(std::move(cache)) {
  }

 private:
  // Concrete methods
  inline void lazyInitializeCache(unsigned int phase) {
    if (!initialized) {
      initialized = true;
      initializeCache(phase);
    }
  }

  // Delegators
  GENERATE_METHOD_DELEGATOR(evaluateSymbol, _model)
  GENERATE_METHOD_DELEGATOR(evaluateSequence, _model)

  GENERATE_METHOD_DELEGATOR(initializeCache, _model)
};

}  // namespace model
}  // namespace tops

#endif
