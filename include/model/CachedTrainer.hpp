/***********************************************************************/
/*  Copyright 2015 ToPS                                                */
/*                                                                     */
/*  This program is free software; you can redistribute it and/or      */
/*  modify it under the terms of the GNU  General Public License as    */
/*  published by the Free Software Foundation; either version 3 of     */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This program is distributed in the hope that it will be useful,    */
/*  but WITHOUT ANY WARRANTY; without even the ied warranty of         */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      */
/*  GNU General Public License for more details.                       */
/*                                                                     */
/*  You should have received a copy of the GNU General Public License  */
/*  along with this program; if not, write to the Free Software        */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,         */
/*  MA 02110-1301, USA.                                                */
/***********************************************************************/

#ifndef TOPS_MODEL_CACHED_TRAINER_
#define TOPS_MODEL_CACHED_TRAINER_

// Standard headers
#include <tuple>
#include <memory>

// ToPS headers
#include "model/Trainer.hpp"

namespace tops {
namespace model {

// Forward declaration
template<template<typename Target> class Decorator,
         typename Model, typename... Params>
class CachedTrainer;

/**
 * @typedef CachedTrainerPtr
 * @brief Alias of pointer to CachedTrainer.
 */
template<template<typename Target> class Decorator,
         typename Model, typename... Params>
using CachedTrainerPtr
  = std::shared_ptr<CachedTrainer<Decorator, Model, Params...>>;

/**
 * @class CachedTrainer
 * @brief TODO
 */
template<template<typename Target> class Decorator,
         typename Model, typename... Params>
class CachedTrainer
    : public SimpleTrainer<Decorator, Model> {
 public:
  // Alias
  using Base = SimpleTrainer<Decorator, Model>;
  using ModelPtr = std::shared_ptr<Model>;

  using Self = CachedTrainer<Decorator, Model, Params...>;
  using SelfPtr = std::shared_ptr<Self>;

  using TrainingSet = typename Base::TrainingSet;

  // Static methods
  template<typename... Args>
  static SelfPtr make(Args&&... args) {
    return SelfPtr(new Self(std::forward<Args>(args)...));
  }

 protected:
  // Instance variables
  std::tuple<Params...> _params;

  // Constructors
  explicit CachedTrainer(Params&&... params)
    : _params(std::forward<Params>(params)...) {
  }

  // Overriden methods
  ModelPtr trainAlt() const override {
    TrainerPtr<Decorator, Model> ptr
      = std::static_pointer_cast<Self>(
        const_cast<Self*>(this)->shared_from_this());

    auto func = [](auto&&... args) {
      return Model::train(std::forward<decltype(args)>(args)...);
    };

    return call(func, ptr, _params);
  }
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_CACHED_TRAINER_
