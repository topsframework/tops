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

#ifndef TOPS_MODEL_PROBABILISTIC_MODEL_CRTP_
#define TOPS_MODEL_PROBABILISTIC_MODEL_CRTP_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Sequence.hpp"
#include "model/ProbabilisticModel.hpp"

// ToPS templates
#include "model/Trainer.tcc"
#include "model/FixedTrainer.tcc"
#include "model/SimpleTrainer.tcc"
#include "model/CachedTrainer.tcc"
#include "model/SimpleGenerator.tcc"
#include "model/SimpleEvaluator.tcc"
#include "model/CachedEvaluator.tcc"
#include "model/SimpleSerializer.tcc"

namespace tops {
namespace model {

// Forward declaration
template<typename Derived>
class ProbabilisticModelCrtp;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef ProbabilisticModelCrtpPtr
 * @brief Alias of pointer to ProbabilisticModelCrtp.
 */
template<typename Derived>
using ProbabilisticModelPtrCrtp
  = std::shared_ptr<ProbabilisticModelCrtp<Derived>>;

/**
 * @class ProbabilisticModelCrtp
 * @brief Implementation of front-end with CRTP to inject methods in  subclasses
 */
template<typename Derived>
class ProbabilisticModelCrtp
    : public std::enable_shared_from_this<ProbabilisticModelCrtp<Derived>>,
      public virtual ProbabilisticModel {
 public:
  // Inner classes
  struct Cache {
    std::vector<double> prefix_sum_array;
  };

  // Alias
  using Base = void;
  using DerivedPtr = std::shared_ptr<Derived>;

  template<template<typename Target> class Decorator>
  using SEPtr = SimpleEvaluatorPtr<Decorator, Derived>;

  template<template<typename Target> class Decorator>
  using CEPtr = CachedEvaluatorPtr<Decorator, Derived>;

  template<template<typename Target> class Decorator>
  using SGPtr = SimpleGeneratorPtr<Decorator, Derived>;

  using SSPtr = SimpleSerializerPtr<Derived>;

  // Static methods
  template<typename... Args>
  static DerivedPtr make(Args&&... args);

  static TrainerPtr<Standard, Derived> standardTrainer();

  static TrainerPtr<Standard, Derived> standardTrainer(DerivedPtr model);

  template<typename Tag, typename... Args>
  static TrainerPtr<Standard, Derived> standardTrainer(Tag, Args&&... args);

  // Overriden methods
  EvaluatorPtr<Standard>
  standardEvaluator(const Standard<Sequence> &sequence,
                    bool cached = false) override;

  GeneratorPtr<Standard>
  standardGenerator(RandomNumberGeneratorPtr rng
                      = RNGAdapter<std::mt19937>::make()) override;

  SerializerPtr serializer(TranslatorPtr translator) override;

  // Purely virtual methods
  virtual Probability evaluateSymbol(SEPtr<Standard> evaluator,
                                     unsigned int pos,
                                     unsigned int phase) const = 0;

  virtual Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                                      unsigned int pos,
                                      unsigned int phase,
                                      const Sequence &context) const = 0;

  // Virtual methods
  virtual void initializeCache(CEPtr<Standard> evaluator,
                               unsigned int phase);

  virtual Probability evaluateSequence(SEPtr<Standard> evaluator,
                                       unsigned int begin,
                                       unsigned int end,
                                       unsigned int phase) const;

  virtual Probability evaluateSymbol(CEPtr<Standard> evaluator,
                                     unsigned int pos,
                                     unsigned int phase) const;
  virtual Probability evaluateSequence(CEPtr<Standard> evaluator,
                                       unsigned int begin,
                                       unsigned int end,
                                       unsigned int phase) const;

  virtual Standard<Sequence> drawSequence(SGPtr<Standard> generator,
                                          unsigned int size,
                                          unsigned int phase) const;

  virtual void serialize(SSPtr serializer,
                         const Serializer::traversal &type);

 private:
  // Concrete methods
  DerivedPtr make_shared();
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                 IMPLEMENTATION
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

/*================================  TRAINER  =================================*/

template<typename Derived>
TrainerPtr<Standard, Derived>
ProbabilisticModelCrtp<Derived>::standardTrainer() {
  return SimpleTrainer<Standard, Derived>::make();
}

template<typename Derived>
TrainerPtr<Standard, Derived>
ProbabilisticModelCrtp<Derived>::standardTrainer(DerivedPtr model) {
  return FixedTrainer<Standard, Derived>::make(model);
}

template<typename Derived>
template<typename Tag, typename... Args>
TrainerPtr<Standard, Derived>
ProbabilisticModelCrtp<Derived>::standardTrainer(Tag, Args&&... args) {
  return CachedTrainer<Standard, Derived, Tag, Args...>::make(
    Tag{}, std::forward<Args>(args)...);
}

/*=================================  OTHERS  =================================*/

template<typename Derived>
template<typename... Args>
auto ProbabilisticModelCrtp<Derived>::make(Args&&... args)
    -> typename ProbabilisticModelCrtp<Derived>::DerivedPtr {
  return DerivedPtr(new Derived(std::forward<Args>(args)...));
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

template<typename Derived>
EvaluatorPtr<Standard> ProbabilisticModelCrtp<Derived>::standardEvaluator(
    const Standard<Sequence> &sequence, bool cached) {
  if (cached)
    return CachedEvaluator<Standard, Derived>::make(make_shared(), sequence);
  return SimpleEvaluator<Standard, Derived>::make(make_shared(), sequence);
}

/*===============================  GENERATOR  ================================*/

template<typename Derived>
GeneratorPtr<Standard> ProbabilisticModelCrtp<Derived>::standardGenerator(
    RandomNumberGeneratorPtr rng) {
  return SimpleGenerator<Standard, Derived>::make(make_shared(), rng);
}

/*===============================  SERIALIZER  ===============================*/

template<typename Derived>
SerializerPtr ProbabilisticModelCrtp<Derived>::serializer(
    TranslatorPtr translator) {
  return SimpleSerializer<Derived>::make(make_shared(), translator);
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

template<typename Derived>
void
ProbabilisticModelCrtp<Derived>::initializeCache(CEPtr<Standard> evaluator,
                                                 unsigned int phase) {
  auto &prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(evaluator->sequence().size() + 1);

  prefix_sum_array[0] = 0;
  for (unsigned int i = 0; i < evaluator->sequence().size(); i++)
    prefix_sum_array[i+1]
      = prefix_sum_array[i] + evaluator->evaluateSymbol(i, phase);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
Probability
ProbabilisticModelCrtp<Derived>::evaluateSequence(SEPtr<Standard> evaluator,
                                                  unsigned int begin,
                                                  unsigned int end,
                                                  unsigned int phase) const {
  double prob = 0;
  for (unsigned int i = begin; i < end; i++)
    prob += evaluator->evaluateSymbol(i, phase);
  return prob;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
Probability
ProbabilisticModelCrtp<Derived>::evaluateSymbol(CEPtr<Standard> evaluator,
                                                unsigned int pos,
                                                unsigned int phase) const {
  return evaluateSymbol(static_cast<SEPtr<Standard>>(evaluator), pos, phase);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
Probability
ProbabilisticModelCrtp<Derived>::evaluateSequence(
    CEPtr<Standard> evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int /* phase */) const {
  auto &prefix_sum_array = evaluator->cache().prefix_sum_array;
  return prefix_sum_array[end] - prefix_sum_array[begin];
}

/*===============================  GENERATOR  ================================*/

template<typename Derived>
Standard<Sequence>
ProbabilisticModelCrtp<Derived>::drawSequence(SGPtr<Standard> generator,
                                              unsigned int size,
                                              unsigned int phase) const {
  Sequence s;
  for (unsigned int k = 0; k < size; k++)
      s.push_back(generator->drawSymbol(k, phase, s));
  return s;
}

/*===============================  SERIALIZER  ===============================*/

template<typename Derived>
void
ProbabilisticModelCrtp<Derived>::serialize(
    SSPtr serializer,
    const Serializer::traversal &/* type */) {
  serializer->translator()->translate(this->make_shared());
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Derived>
std::shared_ptr<Derived> ProbabilisticModelCrtp<Derived>::make_shared() {
  return std::static_pointer_cast<Derived>(
    static_cast<Derived *>(this)->shared_from_this());
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PROBABILISTIC_MODEL_
