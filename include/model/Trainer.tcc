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

#ifndef TOPS_MODEL_TRAINER_
#define TOPS_MODEL_TRAINER_

// Standard headers
#include <memory>

// ToPS templates
#include "model/MemberDelegator.tcc"

namespace tops {
namespace model {

template<template<typename Target> class Decorator, typename Model>
class Trainer;

/**
 * @typedef TrainerPtr
 * @brief Alias of pointer to Trainer.
 */
template<template<typename Target> class Decorator, typename Model>
using TrainerPtr = std::shared_ptr<Trainer<Decorator, Model>>;

/**
 * @class Trainer
 * @brief TODO
 */
template<template<typename Target> class Decorator, typename Model>
class Trainer
    : public std::enable_shared_from_this<Trainer<Decorator, Model>> {
 public:
  // Alias
  using Base = void;
  using ModelPtr = std::shared_ptr<Model>;

  using Self = Trainer<Decorator, Model>;
  using SelfPtr = std::shared_ptr<Self>;

  using TrainingSet = std::vector<Decorator<Sequence>>;

  // Purely virtual methods
  virtual TrainingSet& training_set() = 0;
  virtual const TrainingSet& training_set() const = 0;

  virtual void add_training_set(TrainingSet&& training_set) = 0;
  virtual void add_training_set(const TrainingSet& training_set) = 0;

  virtual void add_training_sequence(Decorator<Sequence>&& sequence) = 0;
  virtual void add_training_sequence(const Decorator<Sequence>& sequence) = 0;

  // Concrete methods
  template<typename... Args>
  ModelPtr train(Args&&... args) const {
    CALL_STATIC_MEMBER_FUNCTION_DELEGATOR(train, std::forward<Args>(args)...);
  }

 protected:
  // Purely virtual methods
  virtual bool delegate() const = 0;
  virtual ModelPtr trainAlt() const = 0;

  GENERATE_STATIC_MEMBER_FUNCTION_DELEGATOR(train, Model)
};

}  // namespace model
}  // namespace tops

#endif
