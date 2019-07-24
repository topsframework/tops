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

#ifndef TOPS_MODEL_PAIR_HIDDEN_MARKOV_MODEL_
#define TOPS_MODEL_PAIR_HIDDEN_MARKOV_MODEL_

// Standard headers
#include <memory>

// Internal headers
#include "model/Probability.hpp"

#include "model/Sequence.hpp"
#include "model/Multiple.hpp"
#include "model/Labeling.hpp"
#include "model/Estimation.hpp"

#include "model/DecodableModelCrtp.hpp"
#include "model/SimpleState.hpp"
#include "model/PairHiddenMarkovModelState.hpp"

namespace tops {
namespace model {

// Forward declaration
class PairHiddenMarkovModel;

/**
 * @typedef HiddenMarkovModelPtr
 * @brief Alias of pointer to HiddenMarkovModel.
 */
using PairHiddenMarkovModelPtr = std::shared_ptr<PairHiddenMarkovModel>;

/**
 * @class PairHiddenMarkovModel
 * @brief TODO
 */
class PairHiddenMarkovModel
    : public DecodableModelCrtp<PairHiddenMarkovModel> {
 public:
  // Tags
  class baum_welch_algorithm {};

  // Aliases
  using Self = PairHiddenMarkovModel;
  using SelfPtr = PairHiddenMarkovModelPtr;
  using Base = DecodableModelCrtp<Self>;

  using Gap1State = typename StateTraits<Self>::Gap1State;
  using Gap2State = typename StateTraits<Self>::Gap2State;

  /*=============================[ CONSTRUCTORS ]=============================*/

  PairHiddenMarkovModel(std::vector<StatePtr> states,
                        size_t state_alphabet_size,
                        size_t observation_alphabet_size);

  /*============================[ STATIC METHODS ]============================*/

  /*-------------------------------( Trainer )--------------------------------*/

  /**
   * Trains (given a set of training arguments, estimate the parameters for)
   * a new instance of the model using the Baum-Welch algorithm.
   * @param trainer Instance of a Trainer (Fixed, Simple or Cached)
   * @param baum_welch_algorithm Tag representing Baum-Welch algorithm
   * @param max_iterations Maximum number of iterations to stop the training
   * @param diff_threshold Minimum threshold of difference to stop the training
   * @return New instance of SelfPtr pointing to a new trained Self
   */
  static SelfPtr train(const TrainerPtr<Multiple, Self>& trainer,
                       baum_welch_algorithm,
                       const PairHiddenMarkovModelPtr& initial_model,
                       size_t max_iterations,
                       Probability diff_threshold);

  /*==========================[ OVERRIDEN METHODS ]===========================*/

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
  Probability evaluateSymbol(SEPtr<Multiple> evaluator,
                             size_t pos,
                             size_t phase) const override;

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
  Probability evaluateSequence(SEPtr<Multiple> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const override;

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
                             size_t phase) const override;

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
                               size_t phase) const override;

  /*---------------------------( CachedEvaluator )----------------------------*/

  /**
   * Lazily initializes the cache of a CachedEvaluator.
   * @param evaluator Instance of CachedEvaluator
   * @param phase Phase of the full sequence
   */
  void initializeCache(CEPtr<Multiple> evaluator,
                       size_t phase) override;

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a **standard symbol** of a CachedEvaluator's **standard sequence**
   * (**with a cache**).
   * @param evaluator Instance of a CachedEvaluator
   * @param pos Position within the full sequence
   * @param phase Phase of the full sequence
   * @return \f$Pr(s[pos])\f$
   */
  Probability evaluateSymbol(CEPtr<Multiple> evaluator,
                             size_t pos,
                             size_t phase) const override;

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
  Probability evaluateSequence(CEPtr<Multiple> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const override;

  /**
   * Lazily initializes the cache of a CachedEvaluator.
   * @param evaluator Instance of CachedEvaluator
   * @param phase Phase of the full labeled sequence
   */
  void initializeCache(CEPtr<Labeling> evaluator,
                       size_t phase) override;

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
                             size_t phase) const override;

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
                               size_t phase) const override;

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
  Multiple<Symbol> drawSymbol(SGPtr<Multiple> generator,
                             size_t pos,
                             size_t phase,
                             const Multiple<Sequence>& context) const override;

  /**
   * Draws (given the trained model, randomly choose) a sequence
   * with a SimpleGenerator (**with no cache**).
   * @param generator Instance of SimpleGenerator
   * @param pos Size of the sequence to be generated
   * @param phase Phase of the sequence to be generated
   * @return \f$x,\ x \in X\f$
   */
  Multiple<Sequence> drawSequence(SGPtr<Multiple> generator,
                                  size_t size,
                                  size_t phase) const override;

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
                              const Labeling<Sequence>& context) const override;

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
                                  size_t phase) const override;

  /*---------------------------( SimpleSerializer )---------------------------*/

  /**
   * Serializes (given the trained model, save its parameters
   * in the disk using some file format) with a SimpleSerializer.
   * @param serializer Instance of SimpleSerializer
   */
  void serialize(const SSPtr serializer) override;

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
  labeling(SLPtr labeler, const Labeler::method& method) const override;

  /*----------------------------( CachedLabeler )-----------------------------*/

  /**
   * Lazily initializes the cache of a CachedLabeler.
   * @param labeler Instance of CachedLabeler
   */
  void initializeCache(CLPtr labeler) override;

  /**
   * Labels (given the trained model, decide the best associated labels
   * accordingly to some criteria) of a CachedLabeler's sequence
   * (**with a cache**).
   * @param labeler Instance of CachedLabeler
   * @param method Criteria to find the best labeling for the sequence
   * @return The labeled sequence with its probability given the model
   */
  Estimation<Labeling<Sequence>>
  labeling(CLPtr labeler, const Labeler::method& method) const override;

  /*---------------------------( SimpleCalculator )---------------------------*/

  /**
   * Calculates associated probabilities (given the model) of a
   * SimpleCalculator's sequence (**without a cache**).
   * @param calculator Instance of SimpleCalculator
   * @param direction Type of calculation
   * @return Probability of the sequence
   */
  Probability calculate(
      SCPtr calculator, const Calculator::direction& direction) const override;

  /*---------------------------( CachedCalculator )---------------------------*/

  /**
   * Lazily initializes the cache of a CachedCalculator.
   * @param calculator Instance of CachedCalculator
   */
  void initializeCache(CCPtr calculator) override;

  /**
   * Calculates associated probabilities (given the model) of a
   * CachedCalculator's sequence (**witho a cache**).
   * @param labeler Instance of CachedCalculator
   * @param direction Type of calculation
   * @return Probability of the sequence
   */
  Probability calculate(
      CCPtr calculator, const Calculator::direction& direction) const override;

  /*===========================[ CONCRETE METHODS ]===========================*/

  // Generator's implementations
  GeneratorReturn<Symbol> drawSymbol(const RandomNumberGeneratorPtr& rng,
                                     size_t pos,
                                     const Sequence& context) const;
  GeneratorReturn<Sequence> drawSequence(const RandomNumberGeneratorPtr& rng,
                                         size_t size) const;

  // Labeler's implementations
  LabelerReturn<2> viterbi(const Multiple<Sequence>& sequences) const;
  LabelerReturn<2> posteriorDecoding(const Multiple<Sequence>& sequences) const;

  // Calculator's implementations
  CalculatorReturn<2> forward(const Multiple<Sequence>& sequences) const;
  CalculatorReturn<2> backward(const Multiple<Sequence>& sequences) const;

  // Helpers
  TraceBackReturn traceBack(const Multiple<Sequence>& sequences,
                            const MultiArray<typename State::Id, 3>& psi) const;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PAIR_HIDDEN_MARKOV_MODEL_
