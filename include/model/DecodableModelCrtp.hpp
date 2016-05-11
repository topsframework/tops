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

// Internal headers
#include "model/State.hpp"
#include "model/Labeler.hpp"
#include "model/Calculator.hpp"
#include "model/SimpleLabeler.hpp"
#include "model/CachedLabeler.hpp"
#include "model/DecodableModel.hpp"
#include "model/SimpleCalculator.hpp"
#include "model/CachedCalculator.hpp"
#include "model/DiscreteIIDModel.hpp"
#include "model/ProbabilisticModelCrtp.hpp"

namespace tops {
namespace model {

// Forward declaration
template<typename Derived>
class DecodableModelCrtp;

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

  using SCPtr = SimpleCalculatorPtr<Derived>;

  using CCPtr = CachedCalculatorPtr<Derived>;

  // Type traits
  using State = typename StateTraits<Derived>::State;
  using StatePtr = std::shared_ptr<State>;

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
  labelingEvaluator(const Labeling<Sequence>& sequence,
                    bool cached = false) override;

  GeneratorPtr<Labeling>
  labelingGenerator(RandomNumberGeneratorPtr rng
                      = RNGAdapter<std::mt19937>::make()) override;

  LabelerPtr labeler(const Sequence& sequence, bool cached = false) override;

  CalculatorPtr calculator(const Sequence& sequence,
                           bool cached = false) override;

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
                                      const Sequence& context) const = 0;
  virtual Labeling<Sequence> drawSequence(SGPtr<Labeling> generator,
                                          unsigned int size,
                                          unsigned int phase) const = 0;

  virtual void initializeCache(CLPtr labeler) = 0;

  virtual Estimation<Labeling<Sequence>>
  labeling(SLPtr labeler, const Labeler::method& method) const = 0;

  virtual Estimation<Labeling<Sequence>>
  labeling(CLPtr labeler, const Labeler::method& method) const = 0;

  virtual void initializeCache(CCPtr calculator) = 0;

  virtual Probability
  calculate(SCPtr calculator, const Calculator::direction& direction) const = 0;

  virtual Probability
  calculate(CCPtr calculator, const Calculator::direction& direction) const = 0;

  virtual void posteriorProbabilities(const Sequence& xs,
                                      Matrix& probabilities) const = 0;

  // Virtual methods
  unsigned int stateAlphabetSize() const;
  unsigned int observationAlphabetSize() const;

  virtual StatePtr state(unsigned int id);
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

}  // namespace model
}  // namespace tops

// Implementation header
#include "model/DecodableModelCrtp.ipp"

#endif  // TOPS_MODEL_DECODABLE_MODEL_
