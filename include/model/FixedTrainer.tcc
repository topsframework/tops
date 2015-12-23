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

#ifndef TOPS_MODEL_FIXED_TRAINER_
#define TOPS_MODEL_FIXED_TRAINER_

// Standard headers
#include <memory>

// ToPS templates
#include "model/Trainer.tcc"

namespace tops {
namespace model {

template<template<typename Target> class Decorator, typename Model>
class FixedTrainer;

/**
 * @typedef FixedTrainerPtr
 * @brief Alias of pointer to FixedTrainer.
 */
template<template<typename Target> class Decorator, typename Model>
using FixedTrainerPtr = std::shared_ptr<FixedTrainer<Decorator, Model>>;

/**
 * @class FixedTrainer
 * @brief TODO
 */
template<template<typename Target> class Decorator, typename Model>
class FixedTrainer
    : public Trainer<Decorator, Model> {
 public:
  // Alias
  using Base = Trainer<Decorator, Model>;
  using ModelPtr = std::shared_ptr<Model>;

  using Self = FixedTrainer<Decorator, Model>;
  using SelfPtr = std::shared_ptr<Self>;

  using TrainingSet = typename Base::TrainingSet;

  // Static methods
  template<typename... Args>
  static SelfPtr make(Args&&... args) {
    return SelfPtr(new Self(std::forward<Args>(args)...));
  }

  // Overriden methods
  TrainingSet& training_set() override {
    return const_cast<TrainingSet&>(
      static_cast<const Self *>(this)->training_set());
  }

  const TrainingSet& training_set() const override {
    throw std::logic_error("Should not be used");
  }

  void add_training_set(TrainingSet&& /* training_set */) override {
    /* do nothing */
  }

  void add_training_set(const TrainingSet& /* training_set */) override {
    /* do nothing */
  }

  void add_training_sequence(
      Decorator<Sequence>&& /* sequence */) override {
    /* do nothing */
  }

  void add_training_sequence(
      const Decorator<Sequence>& /* sequence */) override {
    /* do nothing */
  }

 protected:
  // Instance variables
  ModelPtr _model;

  // Constructors
  explicit FixedTrainer(ModelPtr model) : _model(std::move(model)) {
  }

  // Overriden methods
  bool delegate() const override {
    return false;
  }

  ModelPtr trainAlt() const override {
    return Model::make(*(_model.get()));
  }
};

}  // namespace model
}  // namespace tops

#endif
