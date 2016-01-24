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

#ifndef TOPS_MODEL_SIMPLE_TRAINER_
#define TOPS_MODEL_SIMPLE_TRAINER_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Trainer.hpp"

namespace tops {
namespace model {

// Forward declaration
template<template<typename Target> class Decorator, typename Model>
class SimpleTrainer;

/**
 * @typedef SimpleTrainerPtr
 * @brief Alias of pointer to SimpleTrainer.
 */
template<template<typename Target> class Decorator, typename Model>
using SimpleTrainerPtr = std::shared_ptr<SimpleTrainer<Decorator, Model>>;

/**
 * @class SimpleTrainer
 * @brief TODO
 */
template<template<typename Target> class Decorator, typename Model>
class SimpleTrainer
    : public Trainer<Decorator, Model> {
 public:
  // Alias
  using Base = Trainer<Decorator, Model>;
  using ModelPtr = std::shared_ptr<Model>;

  using Self = SimpleTrainer<Decorator, Model>;
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
    return _training_set;
  }

  void add_training_set(TrainingSet&& training_set) override {
    _training_set.insert(_training_set.end(),
                         std::make_move_iterator(training_set.begin()),
                         std::make_move_iterator(training_set.end()));
  }

  void add_training_set(const TrainingSet& training_set) override {
    _training_set.insert(_training_set.end(),
                         training_set.begin(),
                         training_set.end());
  }

  void add_training_sequence(Decorator<Sequence>&& sequence) override {
    _training_set.push_back(std::move(sequence));
  }

  void add_training_sequence(const Decorator<Sequence>& sequence) override {
    _training_set.push_back(sequence);
  }

 protected:
  // Instance variables
  TrainingSet _training_set;

  // Overriden methods
  bool delegate() const override {
    return true;
  }

  ModelPtr trainAlt() const override {
    throw std::logic_error("Cannot create Model without parameters");
  }
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_SIMPLE_TRAINER_
