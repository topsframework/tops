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

#ifndef TOPS_MODEL_DECODABLE_MODEL_CRTP_
#define TOPS_MODEL_DECODABLE_MODEL_CRTP_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "model/DecodableModel.hpp"
#include "model/ProbabilisticModelState.hpp"

// ToPS templates
#include "model/Labeler.tcc"
#include "model/SimpleLabeler.tcc"
#include "model/CachedLabeler.tcc"
#include "model/ProbabilisticModelCrtp.tcc"

namespace tops {
namespace model {

// Forward declaration
template<typename Derived>
class DecodableModelCrtp;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef DecodableModelCrtpPtr
 * @brief Alias of pointer to DecodableModelCrtp.
 */
template<typename Derived>
using DecodableModelCrtpPtr
  = std::shared_ptr<DecodableModelCrtp<Derived>>;

/**
 * @class DecodableModelCrtp
 * @brief TODO
 */
template<typename Derived>
class DecodableModelCrtp
    : public ProbabilisticModelCrtp<Derived>, public virtual DecodableModel {
 public:
  // Alias
  using Self = DecodableModelCrtp<Derived>;
  using SelfPtr = std::shared_ptr<Self>;

  using Base = ProbabilisticModelCrtp<Derived>;
  using DerivedPtr = std::shared_ptr<Derived>;

  template<template<typename Target> class Decorator>
  using SEPtr = SimpleEvaluatorPtr<Decorator, Derived>;

  template<template<typename Target> class Decorator>
  using CEPtr = CachedEvaluatorPtr<Decorator, Derived>;

  template<template<typename Target> class Decorator>
  using SGPtr = SimpleGeneratorPtr<Decorator, Derived>;

  using SLPtr = SimpleLabelerPtr<Derived>;

  using CLPtr = CachedLabelerPtr<Derived>;

  // Type traits
  using State = typename StateTraits<Derived>::State;
  using StateId = typename StateTraits<Derived>::StateId;
  using StatePtr = typename StateTraits<Derived>::StatePtr;

  // Inner classes
  struct Cache : Base::Cache {
    Matrix alpha, beta, gamma, posterior_decoding;
  };

  // Hidden name method inheritance
  using Base::initializeCache;
  using Base::evaluateSymbol;
  using Base::evaluateSequence;

  using Base::drawSymbol;
  using Base::drawSequence;

  // Static methods
  static TrainerPtr<Labeling, Derived> labelingTrainer();

  static TrainerPtr<Labeling, Derived> labelingTrainer(DerivedPtr model);

  template<typename Tag, typename... Args>
  static TrainerPtr<Labeling, Derived> labelingTrainer(Tag, Args&&... args);

  // Overriden methods
  EvaluatorPtr<Labeling>
  labelingEvaluator(const Labeling<Sequence> &sequence,
                    bool cached = false) override;

  GeneratorPtr<Labeling>
  labelingGenerator(RandomNumberGeneratorPtr rng
                      = RNGAdapter<std::mt19937>::make()) override;

  LabelerPtr labeler(const Sequence &sequence, bool cached = false) override;

  // Purely virtual methods
  virtual void initializeCache(CEPtr<Labeling> evaluator,
                               unsigned int phase) = 0;

  virtual Probability evaluateSymbol(SEPtr<Labeling> evaluator,
                                     unsigned int pos,
                                     unsigned int phase) const = 0;
  virtual Probability evaluateSequence(SEPtr<Labeling> evaluator,
                                       unsigned int begin,
                                       unsigned int end,
                                       unsigned int phase) const = 0;

  virtual Probability evaluateSymbol(CEPtr<Labeling> evaluator,
                                     unsigned int pos,
                                     unsigned int phase) const = 0;
  virtual Probability evaluateSequence(CEPtr<Labeling> evaluator,
                                       unsigned int begin,
                                       unsigned int end,
                                       unsigned int phase) const = 0;

  virtual Labeling<Symbol> drawSymbol(SGPtr<Labeling> generator,
                                      unsigned int pos,
                                      unsigned int phase,
                                      const Sequence &context) const = 0;
  virtual Labeling<Sequence> drawSequence(SGPtr<Labeling> generator,
                                          unsigned int size,
                                          unsigned int phase) const = 0;

  virtual void initializeCache(CLPtr labeler) = 0;

  virtual Estimation<Labeling<Sequence>>
  labeling(SLPtr labeler, const Labeler::method &method) const = 0;

  virtual Estimation<Labeling<Sequence>>
  labeling(CLPtr labeler, const Labeler::method &method) const = 0;

  virtual double forward(const Sequence &s,
                         Matrix &alpha) const = 0;
  virtual double backward(const Sequence &s,
                          Matrix &beta) const = 0;
  virtual void posteriorProbabilities(const Sequence &xs,
                                      Matrix &probabilities) const = 0;

  // Virtual methods
  unsigned int stateAlphabetSize() const;
  unsigned int observationAlphabetSize() const;

  virtual StatePtr state(StateId id);
  virtual std::vector<StatePtr> states();
  virtual const std::vector<StatePtr> states() const;

 protected:
  // Instance variables
  std::vector<StatePtr> _states;
  DiscreteIIDModelPtr _initial_probabilities;
  unsigned int _state_alphabet_size;
  unsigned int _observation_alphabet_size;

  // Constructors
  DecodableModelCrtp(std::vector<StatePtr> states,
                     DiscreteIIDModelPtr _initial_probabilities,
                     unsigned int state_alphabet_size,
                     unsigned int observation_alphabet_size);

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
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename Derived>
DecodableModelCrtp<Derived>::DecodableModelCrtp(
    std::vector<StatePtr> states,
    DiscreteIIDModelPtr initial_probabilities,
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size)
    : _states(std::move(states)),
      _initial_probabilities(initial_probabilities),
      _state_alphabet_size(state_alphabet_size),
      _observation_alphabet_size(observation_alphabet_size) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

/*================================  TRAINER  =================================*/

template<typename Derived>
TrainerPtr<Labeling, Derived>
DecodableModelCrtp<Derived>::labelingTrainer() {
  return SimpleTrainer<Labeling, Derived>::make();
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
TrainerPtr<Labeling, Derived>
DecodableModelCrtp<Derived>::labelingTrainer(DerivedPtr model) {
  return FixedTrainer<Labeling, Derived>::make(model);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
template<typename Tag, typename... Args>
TrainerPtr<Labeling, Derived>
DecodableModelCrtp<Derived>::labelingTrainer(Tag, Args&&... args) {
  return CachedTrainer<Labeling, Derived, Tag, Args...>::make(
    Tag{}, std::forward<Args>(args)...);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

template<typename Derived>
EvaluatorPtr<Labeling> DecodableModelCrtp<Derived>::labelingEvaluator(
    const Labeling<Sequence> &sequence, bool cached) {
  if (cached)
    return CachedEvaluator<Labeling, Derived>::make(make_shared(), sequence);
  return SimpleEvaluator<Labeling, Derived>::make(make_shared(), sequence);
}

/*===============================  GENERATOR  ================================*/

template<typename Derived>
GeneratorPtr<Labeling> DecodableModelCrtp<Derived>::labelingGenerator(
    RandomNumberGeneratorPtr rng) {
  return SimpleGenerator<Labeling, Derived>::make(make_shared(), rng);
}

/*================================  LABELER  =================================*/

template<typename Derived>
LabelerPtr DecodableModelCrtp<Derived>::labeler(
    const Standard<Sequence> &sequence, bool cached) {
  if (cached)
    return CachedLabeler<Derived>::make(make_shared(), sequence);
  return SimpleLabeler<Derived>::make(make_shared(), sequence);
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

template<typename Derived>
unsigned int DecodableModelCrtp<Derived>::stateAlphabetSize() const {
  return _state_alphabet_size;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
unsigned int DecodableModelCrtp<Derived>::observationAlphabetSize() const {
  return _observation_alphabet_size;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
auto DecodableModelCrtp<Derived>::state(StateId id) -> StatePtr {
  return _states[id];
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
auto DecodableModelCrtp<Derived>::states() -> std::vector<StatePtr> {
  return _states;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
auto DecodableModelCrtp<Derived>::states() const -> const std::vector<StatePtr> {
  return _states;
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Derived>
auto DecodableModelCrtp<Derived>::make_shared() -> DerivedPtr {
  return std::static_pointer_cast<Derived>(
    static_cast<Derived *>(this)->shared_from_this());
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_DECODABLE_MODEL_
