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

#ifndef TOPS_MODEL_CACHED_LABELER_
#define TOPS_MODEL_CACHED_LABELER_

// Standard headers
#include <memory>
#include <type_traits>

// Internal headers
#include "model/Labeling.hpp"
#include "model/Sequence.hpp"
#include "model/Estimation.hpp"

namespace tops {
namespace model {

// Forward declaration
template<typename Model>
class CachedLabeler;

/**
 * @typedef CachedLabelerPtr
 * @brief Alias of pointer to CachedLabeler.
 */
template<typename Model>
using CachedLabelerPtr = std::shared_ptr<CachedLabeler<Model>>;

/**
 * @class CachedLabeler
 * @brief TODO
 */
template<typename Model>
class CachedLabeler : public SimpleLabeler<Model> {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;
  using Cache = typename Model::Cache;

  using Base = SimpleLabeler<Model>;
  using Self = CachedLabeler<Model>;
  using SelfPtr = std::shared_ptr<Self>;

  // Static methods
  template<typename... Args>
  static SelfPtr make(Args... args) {
    return std::shared_ptr<Self>(new Self(std::forward<Args>(args)...));
  }

  // Overriden methods
  Estimation<Labeling<Sequence>>
  labeling(const Labeler::method& chosen_method) const override {
    lazyInitializeCache();
    CALL_MEMBER_FUNCTION_DELEGATOR(labeling, chosen_method);
  }

  // Virtual methods
  virtual void initializeCache() const {
    CALL_MEMBER_FUNCTION_DELEGATOR(initializeCache, /* void */);
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
  mutable Cache _cache;
  mutable bool initialized = false;

  // Constructors
  CachedLabeler(
      ModelPtr model, Sequence sequence, Cache cache = Cache())
      : Base(std::move(model), std::move(sequence)), _cache(std::move(cache)) {
  }

  CachedLabeler(ModelPtr model, Sequence sequence,
                std::vector<Sequence> other_sequences, Cache cache = Cache())
      : Base(std::move(model), std::move(sequence), std::move(other_sequences)),
        _cache(std::move(cache)) {
  }

 private:
  // Concrete methods
  inline void lazyInitializeCache() const {
    if (!initialized) {
      initialized = true;
      initializeCache();
    }
  }

  // Delegators
  GENERATE_MEMBER_FUNCTION_DELEGATOR(labeling, _model)
  GENERATE_MEMBER_FUNCTION_DELEGATOR(initializeCache, _model)
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_CACHED_LABELER_
