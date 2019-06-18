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
  using Base = ProbabilisticModelCrtp<Derived>;
  using Self = DecodableModelCrtp<Derived>;
  using SelfPtr = DecodableModelCrtpPtr<Derived>;
  using DerivedPtr = std::shared_ptr<Derived>;

  // Inner structs
  struct Cache : Base::Cache {
    Matrix alpha, beta, gamma, posterior_decoding;
  };

  template<typename Target>
  struct GeneratorReturn {
    Target label;
    std::vector<Target> alignment;
  };

  struct LabelerReturn {
    Probability estimation;
    Sequence label;
    Multiple<Sequence> alignment;
    Matrix matrix;
  };

  struct CalculatorReturn {
    Probability estimation;
    Matrix matrix;
  };

  struct TraceBackReturn {
    Sequence label;
    Multiple<Sequence> alignment;
  };

  // Secretaries
  template<template<typename Target> class Decorator>
  using SEPtr = SimpleEvaluatorPtr<Decorator, Derived>;
  template<template<typename Target> class Decorator>
  using CEPtr = CachedEvaluatorPtr<Decorator, Derived>;

  template<template<typename Target> class Decorator>
  using SGPtr = SimpleGeneratorPtr<Decorator, Derived>;

  using SSPtr = SimpleSerializerPtr<Derived>;

  using SLPtr = SimpleLabelerPtr<Derived>;
  using CLPtr = CachedLabelerPtr<Derived>;

  using SCPtr = SimpleCalculatorPtr<Derived>;
  using CCPtr = CachedCalculatorPtr<Derived>;

  // Type traits
  using State = typename StateTraits<Derived>::State;
  using StatePtr = std::shared_ptr<State>;

  using MatchState  = typename StateTraits<Derived>::MatchState;
  using SilentState = typename StateTraits<Derived>::SilentState;

  // Hidden name method inheritance
  using Base::initializeCache;
  using Base::evaluateSymbol;
  using Base::evaluateSequence;

  using Base::drawSymbol;
  using Base::drawSequence;

  /*============================[ STATIC METHODS ]============================*/

  /*------------------------------( Factories )-------------------------------*/

  /**
   * Factory of Simple Trainers for unsupervised learning of parameters.
   * @return New instance of TrainerPtr<Multiple, Derived>
   */
  static TrainerPtr<Multiple, Derived> unsupervisedTrainer();

  /**
   * Factory of Fixed Trainers for unsupervised learning of parameters.
   * @param model Trained model with predefined parameters
   * @return New instance of TrainerPtr<Multiple, Derived>
   */
  static TrainerPtr<Multiple, Derived> unsupervisedTrainer(DerivedPtr model);

  /**
   * Factory of Cached Trainers for unsupervised learning of parameters.
   * @param tag Tag representing the training algorithm
   * @param params Parameters for the training algorithn chosen
   * @return New instance of TrainerPtr<Multiple, Derived>
   */
  template<typename Tag, typename... Args>
  static TrainerPtr<Multiple, Derived> unsupervisedTrainer(
      Tag /* training_algorithm_tag */, Args&&... args);

  /**
   * Factory of Simple Trainers for supervised learning of parameters.
   * @return New instance of TrainerPtr<Multiple, Derived>
   */
  static TrainerPtr<Labeling, Derived> supervisedTrainer();

  /**
   * Factory of Fixed Trainers for supervised learning of parameters.
   * @param model Trained model with predefined parameters
   * @return New instance of TrainerPtr<Multiple, Derived>
   */
  static TrainerPtr<Labeling, Derived> supervisedTrainer(DerivedPtr model);

  /**
   * Factory of Cached Trainers for supervised learning of parameters.
   * @param tag Tag representing the training algorithm
   * @param params Parameters for the training algorithn chosen
   * @return New instance of TrainerPtr<Multiple, Derived>
   */
  template<typename Tag, typename... Args>
  static TrainerPtr<Labeling, Derived> supervisedTrainer(
      Tag /* training_algorithm_tag */, Args&&... args);

  /*==========================[ OVERRIDEN METHODS ]===========================*/

  /*---------------------------( Decodable Model )----------------------------*/

  EvaluatorPtr<Labeling>
  labelingEvaluator(const Labeling<Sequence>& sequence,
                    bool cached = false) override;

  GeneratorPtr<Labeling>
  labelingGenerator(RandomNumberGeneratorPtr rng
                      = RNGAdapter<std::mt19937>::make()) override;

  LabelerPtr labeler(const Multiple<Sequence>& sequence,
                     bool cached = false) override;

  CalculatorPtr calculator(const Multiple<Sequence>& sequence,
                           bool cached = false) override;

  /*========================[ PURELY VIRTUAL METHODS ]========================*/

  /*---------------------------( SimpleEvaluator )----------------------------*/

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a **labeled symbol** of a SimpleEvaluator's **labeled sequence**
   * (**without a cache**).
   * @param evaluator Instance of a SimpleEvaluator
   * @param pos Position within the full labeled sequence
   * @param phase Phase of the full labeled sequence
   * @return \f$Pr(s[pos])\f$
   */
  virtual Probability evaluateSymbol(SEPtr<Labeling> evaluator,
                                     size_t pos,
                                     size_t phase) const = 0;

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a **labeled subsequence** of a SimpleEvaluator's **labeled sequence**
   * (**without a cache**).
   * @param evaluator Instance of a SimpleEvaluator
   * @param begin Position of the beginning of the labeled subsequence
   * @param end Position of the end of the labeled subsequence, minus 1
   * @param phase Phase of the full labeled sequence
   * @return \f$Pr(s[begin..end-1])\f$
   */
  virtual Probability evaluateSequence(SEPtr<Labeling> evaluator,
                                       size_t begin,
                                       size_t end,
                                       size_t phase) const = 0;

  /*---------------------------( CachedEvaluator )----------------------------*/

  /**
   * Lazily initializes the cache of a CachedEvaluator.
   * @param evaluator Instance of CachedEvaluator
   * @param phase Phase of the full labeled sequence
   */
  virtual void initializeCache(CEPtr<Labeling> evaluator,
                               size_t phase) = 0;

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a **labeled symbol** of a CachedEvaluator's **labeled sequence**
   * (**with a cache**).
   * @param evaluator Instance of a CachedEvaluator
   * @param pos Position within the full labeled sequence
   * @param phase Phase of the full labeled sequence
   * @return \f$Pr(s[pos])\f$
   */
  virtual Probability evaluateSymbol(CEPtr<Labeling> evaluator,
                                     size_t pos,
                                     size_t phase) const = 0;

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a **labeled subsequence** of a CachedEvaluator's **labeled sequence**
   * (**with a cache**).
   * @param evaluator Instance of a CachedEvaluator
   * @param begin Position of the beginning of the labeled subsequence
   * @param end Position of the end of the labeled subsequence, minus 1
   * @param phase Phase of the full labeled sequence
   * @return \f$Pr(s[begin..end-1])\f$
   */
  virtual Probability evaluateSequence(CEPtr<Labeling> evaluator,
                                       size_t begin,
                                       size_t end,
                                       size_t phase) const = 0;

  /*---------------------------( SimpleGenerator )----------------------------*/

  /**
   * Draws (given the trained model, randomly choose) a labeled symbol
   * with a SimpleGenerator (**with no cache**).
   * @param generator Instance of SimpleGenerator
   * @param pos Position of the labeled symbol to be generated
   * @param phase Phase of the labeled sequence in wich the symbol belongs
   * @param context Context to be considered when generating the labeled symbol
   * @return \f$x,\ x \in X\f$
   */
  virtual Labeling<Symbol> drawSymbol(
      SGPtr<Labeling> generator,
      size_t pos,
      size_t phase,
      const Labeling<Sequence>& context) const = 0;

  /**
   * Draws (given the trained model, randomly choose) a labeled sequence
   * with a SimpleGenerator (**with no cache**).
   * @param generator Instance of SimpleGenerator
   * @param pos Size of the labeled sequence to be generated
   * @param phase Phase of the labeled sequence to be generated
   * @return \f$x,\ x \in X\f$
   */
  virtual Labeling<Sequence> drawSequence(SGPtr<Labeling> generator,
                                          size_t size,
                                          size_t phase) const = 0;

  /*----------------------------( SimpleLabeler )-----------------------------*/

  /**
   * Labels (given the trained model, decide the best associated labels
   * accordingly to some criteria) of a SimpleLabeler's sequence
   * (**without a cache**).
   * @param labeler Instance of SimpleLabeler
   * @param method Criteria to find the best labeling for the sequence
   * @return The labeled sequence with its probability given the model
   */
  virtual Estimation<Labeling<Sequence>>
  labeling(SLPtr labeler, const Labeler::method& method) const = 0;

  /*----------------------------( CachedLabeler )-----------------------------*/

  /**
   * Lazily initializes the cache of a CachedLabeler.
   * @param labeler Instance of CachedLabeler
   */
  virtual void initializeCache(CLPtr labeler) = 0;

  /**
   * Labels (given the trained model, decide the best associated labels
   * accordingly to some criteria) of a CachedLabeler's sequence
   * (**with a cache**).
   * @param labeler Instance of CachedLabeler
   * @param method Criteria to find the best labeling for the sequence
   * @return The labeled sequence with its probability given the model
   */
  virtual Estimation<Labeling<Sequence>>
  labeling(CLPtr labeler, const Labeler::method& method) const = 0;

  /*---------------------------( SimpleCalculator )---------------------------*/

  /**
   * Calculates associated probabilities (given the model) of a
   * SimpleCalculator's sequence (**without a cache**).
   * @param calculator Instance of SimpleCalculator
   * @param direction Type of calculation
   * @return Probability of the sequence
   */
  virtual Probability
  calculate(SCPtr calculator, const Calculator::direction& direction) const = 0;

  /*---------------------------( CachedCalculator )---------------------------*/

  /**
   * Lazily initializes the cache of a CachedCalculator.
   * @param calculator Instance of CachedCalculator
   */
  virtual void initializeCache(CCPtr calculator) = 0;

  /**
   * Calculates associated probabilities (given the model) of a
   * CachedCalculator's sequence (**witho a cache**).
   * @param labeler Instance of CachedCalculator
   * @param direction Type of calculation
   * @return Probability of the sequence
   */
  virtual Probability
  calculate(CCPtr calculator, const Calculator::direction& direction) const = 0;

  /*============================[ VIRTUAL METHODS ]===========================*/

  /**
   * Gets the model's state alphabet size.
   * @return \f$|Y|\f$
   */
  size_t stateAlphabetSize() const;

  /**
   * Gets the model's observation alphabet size.
   * @return \f$|X|\f$
   */
  size_t observationAlphabetSize() const;

  /**
   * Gets the state with a given ID.
   * @return \f$y_i\f$
   */
  StatePtr state(typename State::Id id);

  /**
   * Gets a modifiable vector of std::shared_ptr<State>.
   * @return \f$Yf$
   */
  std::vector<StatePtr> states();

  /**
   * Gets a non-modifiable vector of std::shared_ptr<State>.
   * @return \f$Yf$
   */
  const std::vector<StatePtr> states() const;

 protected:
  // Instance variables
  std::vector<StatePtr> _states;
  size_t _state_alphabet_size;
  size_t _observation_alphabet_size;

  Symbol _gap = _observation_alphabet_size;

  typename State::Id _begin_id = 0;
  typename State::Id _end_id = _state_alphabet_size-1;

  // Constructors
  DecodableModelCrtp(std::vector<StatePtr> states,
                     size_t state_alphabet_size,
                     size_t observation_alphabet_size);

 private:
  // Concrete methods
  DerivedPtr make_shared();
};

}  // namespace model
}  // namespace tops

// Implementation header
#include "model/DecodableModelCrtp.ipp"

#endif  // TOPS_MODEL_DECODABLE_MODEL_
