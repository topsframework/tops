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

#ifndef TOPS_MODEL_CACHED_LABELER_IMPL_
#define TOPS_MODEL_CACHED_LABELER_IMPL_

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
class CachedLabeler;

/**
 * @typedef CachedLabelerPtr
 * @brief Alias of pointer to CachedLabeler.
 */
template<template<typename Target> class Decorator, typename Model>
using CachedLabelerPtr = std::shared_ptr<CachedLabeler<Decorator, Model>>;

/**
 * @class CachedLabeler
 * @brief TODO
 */
template<template<typename Target> class Decorator, typename Model>
class CachedLabeler : public SimpleLabeler<Decorator, Model> {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;
  using Cache = typename Model::Cache;

  using Base = SimpleLabeler<Decorator, Model>;
  using Self = CachedLabeler<Decorator, Model>;
  using SelfPtr = std::shared_ptr<Self>;

  // Static methods
  template<typename... Ts>
  static CachedLabelerPtr<Decorator, Model> make(Ts... args) {
    return std::shared_ptr<Self>(new Self(std::forward<Ts>(args)...));
  }

  // Virtual methods
  virtual void initializeCache(unsigned int phase) {
    CALL_MEMBER_FUNCTION_DELEGATOR(initializeCache, phase);
  }

  // Concrete methods

  Estimation<Labeling<Sequence>> labeling(
      Labeling<Sequence>::Method method) const override {
    const_cast<Self *>(this)->lazyInitializeCache(0);
    CALL_MEMBER_FUNCTION_DELEGATOR(labeling, method);
  }

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
  CachedLabeler(
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
  GENERATE_MEMBER_FUNCTION_DELEGATOR(labeling, _model)
  GENERATE_MEMBER_FUNCTION_DELEGATOR(initializeCache, _model)
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_CACHED_LABELER_IMPL_
