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

#ifndef TOPS_MODEL_HIDDEN_MARKOV_MODEL_
#define TOPS_MODEL_HIDDEN_MARKOV_MODEL_

// Standard headers
#include <memory>
#include <vector>
#include <utility>

// Internal headers
#include "model/Matrix.hpp"
#include "model/SimpleState.hpp"
#include "model/DecodableModelCrtp.hpp"
#include "model/HiddenMarkovModelState.hpp"

namespace tops {
namespace model {

// Forward declaration
class HiddenMarkovModel;

/**
 * @typedef HiddenMarkovModelPtr
 * @brief Alias of pointer to HiddenMarkovModel.
 */
using HiddenMarkovModelPtr = std::shared_ptr<HiddenMarkovModel>;

/**
 * @class HiddenMarkovModel
 * @brief TODO
 */
class HiddenMarkovModel
    : public DecodableModelCrtp<HiddenMarkovModel> {
 public:
  // Tags
  class baum_welch_algorithm {};
  class maximum_likehood_algorithm {};

  // Aliases
  using Self = HiddenMarkovModel;
  using SelfPtr = HiddenMarkovModelPtr;
  using Base = DecodableModelCrtp<Self>;
  using Cache = Base::Cache;

  // Type traits
  using State = typename StateTraits<Self>::State;
  using StatePtr = std::shared_ptr<State>;

  /*=============================[ CONSTRUCTORS ]=============================*/

  HiddenMarkovModel(std::vector<StatePtr> states,
                    DiscreteIIDModelPtr initial_probability,
                    unsigned int state_alphabet_size,
                    unsigned int observation_alphabet_size);

  /*============================[ STATIC METHODS ]============================*/

  // Trainer
  static SelfPtr train(TrainerPtr<Standard, Self> trainer,
                       baum_welch_algorithm,
                       HiddenMarkovModelPtr initial_model,
                       unsigned int maxiterations,
                       double diff_threshold);
  static SelfPtr train(TrainerPtr<Labeling, Self> trainer,
                       maximum_likehood_algorithm,
                       unsigned int state_alphabet_size,
                       unsigned int observation_alphabet_size,
                       double pseudocont);

  /*==========================[ OVERRIDEN METHODS ]===========================*/
  /*-------------------------( Probabilistic Model )--------------------------*/

  // SimpleEvaluator
  Probability evaluateSymbol(SEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(SEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  // CachedEvaluator
  void initializeCache(CEPtr<Standard> evaluator,
                       unsigned int phase) override;
  Probability evaluateSymbol(CEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(CEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  // SimpleGenerator
  Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                              unsigned int pos,
                              unsigned int phase,
                              const Sequence& context) const override;
  Standard<Sequence> drawSequence(SGPtr<Standard> generator,
                                  unsigned int size,
                                  unsigned int phase) const override;

  // SimpleSerializer
  void serialize(SSPtr serializer) override;

  /*==========================[ OVERRIDEN METHODS ]===========================*/
  /*---------------------------( Decodable Model )----------------------------*/

  // SimpleEvaluator
  Probability evaluateSymbol(SEPtr<Labeling> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(SEPtr<Labeling> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  // CachedEvaluator
  void initializeCache(CEPtr<Labeling> evaluator,
                       unsigned int phase) override;
  Probability evaluateSymbol(CEPtr<Labeling> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(CEPtr<Labeling> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  // SimpleGenerator
  Labeling<Symbol> drawSymbol(SGPtr<Labeling> generator,
                              unsigned int pos,
                              unsigned int phase,
                              const Sequence& context) const override;
  Labeling<Sequence> drawSequence(SGPtr<Labeling> generator,
                                  unsigned int size,
                                  unsigned int phase) const override;

  // SimpleLabeler
  Estimation<Labeling<Sequence>> labeling(
      SLPtr labeler,
      const Labeler::method& method) const override;

  // CachedLabeler
  void initializeCache(CLPtr labeler) override;
  Estimation<Labeling<Sequence>> labeling(
      CLPtr labeler,
      const Labeler::method& method) const override;

  // SimpleCalculator
  Probability calculate(SCPtr calculator,
                        const Calculator::direction& direction) const override;

  // CachedCalculator
  void initializeCache(CCPtr calculator) override;
  Probability calculate(CCPtr calculator,
                        const Calculator::direction& direction) const override;

  // Others
  void posteriorProbabilities(const Sequence& sequence,
                              Matrix& probabilities) const override;

 private:
  /*==========================[ CONCRETE METHODS ]============================*/

  // Labeler's helpers
  Estimation<Labeling<Sequence>>
  viterbi(const Sequence& xs, Matrix& gamma) const;

  Estimation<Labeling<Sequence>>
  posteriorDecoding(const Sequence& xs, Matrix& probabilities) const;

  // Calculator's helpers
  Probability backward(const Sequence& sequence, Matrix& beta) const;
  Probability forward(const Sequence& sequence, Matrix& alpha) const;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_HIDDEN_MARKOV_MODEL_
