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

#ifndef TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_
#define TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_

// Standard headers
#include <memory>
#include <vector>
#include <utility>

// Internal headers
#include "model/Matrix.hpp"
#include "model/Labeling.hpp"
#include "model/Sequence.hpp"
#include "model/Estimation.hpp"
#include "model/Probability.hpp"
#include "model/SimpleState.hpp"
#include "model/DiscreteIIDModel.hpp"
#include "model/GeneralizedHiddenMarkovModelState.hpp"

#include "model/Trainer.hpp"
#include "model/FixedTrainer.hpp"
#include "model/SimpleTrainer.hpp"
#include "model/CachedTrainer.hpp"

#include "model/Evaluator.hpp"
#include "model/SimpleEvaluator.hpp"
#include "model/CachedEvaluator.hpp"

#include "model/Generator.hpp"
#include "model/SimpleGenerator.hpp"

#include "model/Serializer.hpp"
#include "model/SimpleSerializer.hpp"

#include "model/Labeler.hpp"
#include "model/SimpleLabeler.hpp"
#include "model/CachedLabeler.hpp"

#include "model/Calculator.hpp"
#include "model/SimpleCalculator.hpp"
#include "model/CachedCalculator.hpp"

#include "model/RandomNumberGenerator.hpp"
#include "model/RandomNumberGeneratorAdapter.hpp"

namespace tops {
namespace model {

// Forward declaration
class GeneralizedHiddenMarkovModel;

/**
 * @typedef GeneralizedHiddenMarkovModelPtr
 * @brief Alias of pointer to GeneralizedHiddenMarkovModel.
 */
using GeneralizedHiddenMarkovModelPtr = std::shared_ptr<GeneralizedHiddenMarkovModel>;

/**
 * @class GeneralizedHiddenMarkovModel
 * @brief TODO
 */
class GeneralizedHiddenMarkovModel
    : public std::enable_shared_from_this<GeneralizedHiddenMarkovModel> {
 public:
  // Tags
  class maximum_likelihood_algorithm {};

  // Aliases
  using Self = GeneralizedHiddenMarkovModel;
  using SelfPtr = GeneralizedHiddenMarkovModelPtr;
  using Base = void;

  template<typename T> using Alignment = std::vector<T>;
  template<typename T> using LabelingAlignment = Labeling<std::vector<T>>;

  // Inner structs
  struct Cache {
    std::vector<Probability> prefix_sum_array;
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
    Sequences alignment;
    Matrix matrix;
  };

  struct CalculatorReturn {
    Probability estimation;
    Matrix matrix;
  };

  struct TraceBackReturn {
    Sequence label;
    Sequences alignment;
  };

  // Secretaries
  template<template<typename Target> class Decorator>
  using SEPtr = SimpleEvaluatorPtr<Decorator, Self>;
  template<template<typename Target> class Decorator>
  using CEPtr = CachedEvaluatorPtr<Decorator, Self>;

  template<template<typename Target> class Decorator>
  using SGPtr = SimpleGeneratorPtr<Decorator, Self>;

  using SSPtr = SimpleSerializerPtr<Self>;

  using SLPtr = SimpleLabelerPtr<Self>;
  using CLPtr = CachedLabelerPtr<Self>;

  using SCPtr = SimpleCalculatorPtr<Self>;
  using CCPtr = CachedCalculatorPtr<Self>;

  // Type traits
  using State = typename StateTraits<Self>::State;
  using StatePtr = std::shared_ptr<State>;

  using MatchState  = typename StateTraits<Self>::MatchState;
  using SilentState = typename StateTraits<Self>::SilentState;

  /*=============================[ CONSTRUCTORS ]=============================*/

  GeneralizedHiddenMarkovModel(
      std::vector<StatePtr> states,
      size_t state_alphabet_size,
      size_t observation_alphabet_size,
      size_t num_phases,
      size_t max_backtracking);

  /*============================[ STATIC METHODS ]============================*/

  /*-----------------------------( Constructors )-----------------------------*/

  /**
   * Make a std::shared_ptr<Self>.
   * @param args Any arguments, forwared to Self's constructor
   * @return New instance of new std::shared_ptr<Self>
   */
  template<typename... Args>
  static SelfPtr make(Args&&... args) {
    return std::make_shared<Self>(std::forward<Args>(args)...);
  }

  /*------------------------------( Factories )-------------------------------*/

  /**
   * Factory of Simple Trainers for unsupervised learning of parameters.
   * @return New instance of TrainerPtr<Alignment, Self>
   */
  static TrainerPtr<Alignment, Self> unsupervisedTrainer(){
    return SimpleTrainer<Alignment, Self>::make();
  }

  /**
   * Factory of Fixed Trainers for unsupervised learning of parameters.
   * @param model Trained model with predefined parameters
   * @return New instance of TrainerPtr<Alignment, Self>
   */
  static TrainerPtr<Alignment, Self> unsupervisedTrainer(SelfPtr model) {
    return FixedTrainer<Alignment, Self>::make(model);
  }

  /**
   * Factory of Cached Trainers for unsupervised learning of parameters.
   * @param tag Tag representing the training algorithm
   * @param params Parameters for the training algorithn chosen
   * @return New instance of TrainerPtr<Alignment, Self>
   */
  template<typename Tag, typename... Args>
  static TrainerPtr<Alignment, Self> unsupervisedTrainer(
      Tag /* training_algorithm_tag */, Args&&... args) {
    return CachedTrainer<Alignment, Self, Tag, Args...>::make(
        Tag{}, std::forward<Args>(args)...);
  }

  /**
   * Factory of Simple Trainers for supervised learning of parameters.
   * @return New instance of TrainerPtr<Standard, Derived>
   */
  static TrainerPtr<Labeling, Self> supervisedTrainer() {
    return SimpleTrainer<Labeling, Self>::make();
  }

  /**
   * Factory of Fixed Trainers for supervised learning of parameters.
   * @param model Trained model with predefined parameters
   * @return New instance of TrainerPtr<Standard, Self>
   */
  static TrainerPtr<Labeling, Self> supervisedTrainer(SelfPtr model) {
    return FixedTrainer<Labeling, Self>::make(model);
  }

  /**
   * Factory of Cached Trainers for supervised learning of parameters.
   * @param tag Tag representing the training algorithm
   * @param params Parameters for the training algorithn chosen
   * @return New instance of TrainerPtr<Standard, Self>
   */
  template<typename Tag, typename... Args>
  static TrainerPtr<Labeling, Self> supervisedTrainer(
      Tag /* training_algorithm_tag */, Args&&... args) {
    return CachedTrainer<Labeling, Self, Tag, Args...>::make(
        Tag{}, std::forward<Args>(args)...);
  }

  /*-------------------------------( Trainer )--------------------------------*/

  /**
   * Trains (given a set of training arguments, estimate the parameters for)
   * a new instance of the model using the Maximum-Likelihood algorithm.
   * @param trainer Instance of a Trainer (Fixed, Simple or Cached)
   * @param maximum_likelihood_algorithm Tag representing Maximum-Likelihood
   * @param initial_model Maximum number of iterations to stop the training
   * @param pseudo_counter Minimum count for emissions and transitions
   * @return New instance of SelfPtr pointing to a new trained Self
   */
  static SelfPtr train(const TrainerPtr<Labeling, Self>& trainer,
                       maximum_likelihood_algorithm /* tag */,
                       const GeneralizedHiddenMarkovModelPtr& initial_model,
                       size_t pseudo_counter);

  /*==========================[ CONCRETE METHODS ]============================*/

  /*------------------------------( Factories )-------------------------------*/

  /**
   * Factory of Simple/Cached Evaluators.
   * @tparam Decorator Type of sequence (standard or labeled) being evaluated
   * @param sequence Sequence to be evaluated
   * @param cached Type of Evaluator (Simple or Cached)
   * @return New instance of EvaluatorPtr<Standard>
   */
  template<template<typename Target> class Decorator>
  EvaluatorPtr<Decorator> evaluator(const Decorator<Sequence>& sequence,
                                    bool cached = false) {
    auto self = shared_from_this();
    return cached ? CachedEvaluator<Decorator, Self>::make(self, sequence)
                  : SimpleEvaluator<Decorator, Self>::make(self, sequence);
  }

  /**
   * Factory of Simple Generators.
   * @tparam Decorator Type of sequence (standard or labeled) being evaluated
   * @param rng Random Number Generator
   * @return New instance of EvaluatorPtr<Standard>
   */
  template<template<typename Target> class Decorator>
  GeneratorPtr<Decorator> generator(RandomNumberGeneratorPtr rng
                                      = RNGAdapter<std::mt19937>::make()) {
    auto self = shared_from_this();
    return SimpleGenerator<Decorator, Self>::make(self, rng);
  }

  /**
   * Factory of Simple Serializers.
   * @param translator Visitor that serializes the files
   * @return New instance of SerializerPtr
   */
  SerializerPtr serializer(TranslatorPtr translator) {
    auto self = shared_from_this();
    return SimpleSerializer<Self>::make(self, std::move(translator));
  }

  /**
   * Factory of Simple/Cached Labelers
   * @param sequence Input sequence to be labeled
   * @param cached Type of Labeler (cached or non-cached)
   * @return New instance of LabelerPtr
   */
  LabelerPtr labeler(const Sequence& sequence, bool cached = false) {
    auto self = shared_from_this();
    return cached ? CachedLabeler<Self>::make(self, sequence)
                  : SimpleLabeler<Self>::make(self, sequence);
  }

  /**
   * Factory of Simple/Cached Calculators
   * @param sequence Input sequence that will be used for calculations
   * @param cached Type of Calculator (cached or non-cached)
   * @return New instance of CalculatorPtr
   */
  CalculatorPtr calculator(const Sequence& sequence, bool cached = false) {
    auto self = shared_from_this();
    return cached ? CachedCalculator<Self>::make(self, sequence)
                  : SimpleCalculator<Self>::make(self, sequence);
  }

  /*---------------------------( SimpleEvaluator )----------------------------*/

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a **standard symbol** of a SimpleEvaluator's **standard sequence**
   * (**without a cache**).
   * @param evaluator Instance of a SimpleEvaluator
   * @param pos Position within the full sequence
   * @param phase Phase of the full sequence
   * @return \f$Pr(s[pos])\f$
   */
  Probability evaluateSymbol(SEPtr<Standard> evaluator,
                             size_t pos,
                             size_t phase) const;

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a **standard subsequence** of a SimpleEvaluator's **standard sequence**
   * (**without a cache**).
   * @param evaluator Instance of a SimpleEvaluator
   * @param begin Position of the beginning of the subsequence
   * @param end Position of the end of the subsequence, minus 1
   * @param phase Phase of the full sequence
   * @return \f$Pr(s[begin..end-1])\f$
   */
  Probability evaluateSequence(SEPtr<Standard> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const;

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a **labeled symbol** of a SimpleEvaluator's **labeled sequence**
   * (**without a cache**).
   * @param evaluator Instance of a SimpleEvaluator
   * @param pos Position within the full labeled sequence
   * @param phase Phase of the full labeled sequence
   * @return \f$Pr(s[pos])\f$
   */
  Probability evaluateSymbol(SEPtr<Labeling> evaluator,
                             size_t pos,
                             size_t phase) const;

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
  Probability evaluateSequence(SEPtr<Labeling> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const;

  /*---------------------------( CachedEvaluator )----------------------------*/

  /**
   * Lazily initializes the cache of a CachedEvaluator.
   * @param evaluator Instance of CachedEvaluator
   * @param phase Phase of the full sequence
   */
  void initializeCache(CEPtr<Standard> evaluator,
                       size_t phase);

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a **standard symbol** of a CachedEvaluator's **standard sequence**
   * (**with a cache**).
   * @param evaluator Instance of a CachedEvaluator
   * @param pos Position within the full sequence
   * @param phase Phase of the full sequence
   * @return \f$Pr(s[pos])\f$
   */
  Probability evaluateSymbol(CEPtr<Standard> evaluator,
                             size_t pos,
                             size_t phase) const;

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a **standard subsequence** of a CachedEvaluator's **standard sequence**
   * (**with a cache**).
   * @param evaluator Instance of a CachedEvaluator
   * @param begin Position of the beginning of the subsequence
   * @param end Position of the end of the subsequence, minus 1
   * @param phase Phase of the full sequence
   * @return \f$Pr(s[begin..end-1])\f$
   */
  Probability evaluateSequence(CEPtr<Standard> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const;

  /**
   * Lazily initializes the cache of a CachedEvaluator.
   * @param evaluator Instance of CachedEvaluator
   * @param phase Phase of the full labeled sequence
   */
  void initializeCache(CEPtr<Labeling> evaluator,
                       size_t phase);

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a **labeled symbol** of a CachedEvaluator's **labeled sequence**
   * (**with a cache**).
   * @param evaluator Instance of a CachedEvaluator
   * @param pos Position within the full labeled sequence
   * @param phase Phase of the full labeled sequence
   * @return \f$Pr(s[pos])\f$
   */
  Probability evaluateSymbol(CEPtr<Labeling> evaluator,
                             size_t pos,
                             size_t phase) const;

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
  Probability evaluateSequence(CEPtr<Labeling> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const;

  /*---------------------------( SimpleGenerator )----------------------------*/

  /**
   * Draws (given the trained model, randomly choose) a symbol
   * with a SimpleGenerator (**with no cache**).
   * @param generator Instance of SimpleGenerator
   * @param pos Position of the symbol to be generated
   * @param phase Phase of the sequence in wich the symbol belongs
   * @param context Context to be considered when generating the symbol
   * @return \f$x,\ x \in X\f$
   */
  Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                             size_t pos,
                             size_t phase,
                             const Sequence& context) const;

  /**
   * Draws (given the trained model, randomly choose) a sequence
   * with a SimpleGenerator (**with no cache**).
   * @param generator Instance of SimpleGenerator
   * @param pos Size of the sequence to be generated
   * @param phase Phase of the sequence to be generated
   * @return \f$x,\ x \in X\f$
   */
  Standard<Sequence> drawSequence(SGPtr<Standard> generator,
                                  size_t size,
                                  size_t phase) const;

  /**
   * Draws (given the trained model, randomly choose) a labeled symbol
   * with a SimpleGenerator (**with no cache**).
   * @param generator Instance of SimpleGenerator
   * @param pos Position of the labeled symbol to be generated
   * @param phase Phase of the labeled sequence in wich the symbol belongs
   * @param context Context to be considered when generating the labeled symbol
   * @return \f$x,\ x \in X\f$
   */
  Labeling<Symbol> drawSymbol(SGPtr<Labeling> generator,
                              size_t pos,
                              size_t phase,
                              const Sequence& context) const;

  /**
   * Draws (given the trained model, randomly choose) a labeled sequence
   * with a SimpleGenerator (**with no cache**).
   * @param generator Instance of SimpleGenerator
   * @param pos Size of the labeled sequence to be generated
   * @param phase Phase of the labeled sequence to be generated
   * @return \f$x,\ x \in X\f$
   */
  Labeling<Sequence> drawSequence(SGPtr<Labeling> generator,
                                  size_t size,
                                  size_t phase) const;

  /*---------------------------( SimpleSerializer )---------------------------*/

  /**
   * Serializes (given the trained model, save its parameters
   * in the disk using some file format) with a SimpleSerializer.
   * @param serializer Instance of SimpleSerializer
   */
  void serialize(const SSPtr& serializer);

  /*----------------------------( SimpleLabeler )-----------------------------*/

  /**
   * Labels (given the trained model, decide the best associated labels
   * accordingly to some criteria) of a SimpleLabeler's sequence
   * (**without a cache**).
   * @param labeler Instance of SimpleLabeler
   * @param method Criteria to find the best labeling for the sequence
   * @return The labeled sequence with its probability given the model
   */
  Estimation<Labeling<Sequence>>
  labeling(SLPtr labeler, const Labeler::method& method) const;

  /*----------------------------( CachedLabeler )-----------------------------*/

  /**
   * Lazily initializes the cache of a CachedLabeler.
   * @param labeler Instance of CachedLabeler
   */
  void initializeCache(CLPtr labeler);

  /**
   * Labels (given the trained model, decide the best associated labels
   * accordingly to some criteria) of a CachedLabeler's sequence
   * (**with a cache**).
   * @param labeler Instance of CachedLabeler
   * @param method Criteria to find the best labeling for the sequence
   * @return The labeled sequence with its probability given the model
   */
  Estimation<Labeling<Sequence>>
  labeling(CLPtr labeler, const Labeler::method& method) const;

  /*---------------------------( SimpleCalculator )---------------------------*/

  /**
   * Calculates associated probabilities (given the model) of a
   * SimpleCalculator's sequence (**without a cache**).
   * @param calculator Instance of SimpleCalculator
   * @param direction Type of calculation
   * @return Probability of the sequence
   */
  Probability
  calculate(SCPtr calculator, const Calculator::direction& direction) const;

  /*---------------------------( CachedCalculator )---------------------------*/

  /**
   * Lazily initializes the cache of a CachedCalculator.
   * @param calculator Instance of CachedCalculator
   */
  void initializeCache(CCPtr calculator);

  /**
   * Calculates associated probabilities (given the model) of a
   * CachedCalculator's sequence (**witho a cache**).
   * @param labeler Instance of CachedCalculator
   * @param direction Type of calculation
   * @return Probability of the sequence
   */
  Probability
  calculate(CCPtr calculator, const Calculator::direction& direction) const;

  /*--------------------------------( Getters )-------------------------------*/

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

  /*----------------------------( Implementations )---------------------------*/

  // Generator's implementations
  GeneratorReturn<Symbol> drawSymbol(const RandomNumberGeneratorPtr& rng,
                                     size_t pos,
                                     const Sequence& context) const;
  GeneratorReturn<Sequence> drawSequence(const RandomNumberGeneratorPtr& rng,
                                         size_t size) const;

  // Labeler's implementations
  LabelerReturn viterbi(const Sequences& sequences) const;
  LabelerReturn posteriorDecoding(const Sequences& sequences) const;

  // Calculator's implementations
  CalculatorReturn forward(const Sequences& sequences) const;
  CalculatorReturn backward(const Sequences& sequences) const;

  // Helpers
  bool segmentIsViable(const Sequence& sequence,
                       size_t begin,
                       size_t end,
                       const StatePtr& state) const;

  TraceBackReturn traceBack(const Sequences& sequences,
                            const MultiArray<typename State::Id, 2>& psi,
                            const MultiArray<size_t, 2>& phi) const;

 protected:
  // Instance variables
  std::vector<StatePtr> _states;
  size_t _state_alphabet_size;
  size_t _observation_alphabet_size;
  size_t _num_phases;
  size_t _max_backtracking;

  Symbol _gap = _observation_alphabet_size;

  typename State::Id _begin_id = 0;
  typename State::Id _end_id = _state_alphabet_size-1;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_
