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

// ToPS headers
#include "model/Matrix.hpp"
#include "model/GeneralizedHiddenMarkovModelState.hpp"
#include "model/GeneralizedHiddenMarkovModelSignalState.hpp"
#include "model/GeneralizedHiddenMarkovModelExplicitDurationState.hpp"

// ToPS templates
#include "model/DecodableModelCrtp.tcc"

namespace tops {
namespace model {

class GeneralizedHiddenMarkovModel;

/**
 * @typedef GeneralizedHiddenMarkovModelPtr
 * @brief Alias of pointer to GeneralizedHiddenMarkovModel.
 */
using GeneralizedHiddenMarkovModelPtr
  = std::shared_ptr<GeneralizedHiddenMarkovModel>;

/**
 * @class GeneralizedHiddenMarkovModel
 * @brief TODO
 */
class GeneralizedHiddenMarkovModel
    : public DecodableModelCrtp<GeneralizedHiddenMarkovModel> {
 public:
  // Alias
  using Base = DecodableModelCrtp<GeneralizedHiddenMarkovModel>;

  // Hidden name method inheritance
  using Base::evaluateSequence;
  using Base::drawSequence;

  struct Cache : Base::Cache {
    std::vector<EvaluatorPtr<Standard>> observation_evaluators;
  };

  // Constructors
  GeneralizedHiddenMarkovModel(
      std::vector<GeneralizedHiddenMarkovModelStatePtr> states,
      DiscreteIIDModelPtr initial_probability,
      unsigned int state_alphabet_size,
      unsigned int observation_alphabet_size,
      unsigned int max_backtracking = 100);

  // Overriden methods
  void initializeCache(CEPtr<Standard> evaluator,
                       unsigned int phase) override;
  Probability evaluateSymbol(CEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(CEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  Probability evaluateSymbol(SEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(SEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  void initializeCache(CEPtr<Labeling> evaluator,
                       unsigned int phase) override;
  Probability evaluateSymbol(CEPtr<Labeling> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(CEPtr<Labeling> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  Probability evaluateSymbol(SEPtr<Labeling> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(SEPtr<Labeling> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                              unsigned int pos,
                              unsigned int phase,
                              const Sequence &context) const override;

  Labeling<Symbol> drawSymbol(SGPtr<Labeling> generator,
                              unsigned int pos,
                              unsigned int phase,
                              const Sequence &context) const override;
  Labeling<Sequence> drawSequence(SGPtr<Labeling> generator,
                                  unsigned int size,
                                  unsigned int phase) const override;

  void initializeCache(CLPtr labeler) override;
  Estimation<Labeling<Sequence>> labeling(
      CLPtr labeler,
      const Labeler::method &method) const override;

  Estimation<Labeling<Sequence>> labeling(
      SLPtr labeler,
      const Labeler::method &method) const override;

  double forward(const Sequence &sequence, Matrix &alpha) const override;

  double backward(const Sequence &sequence, Matrix &beta) const override;

  void posteriorProbabilities(const Sequence &sequence,
                              Matrix &probabilities) const override;

 protected:
  // Instance variables
  std::vector<GeneralizedHiddenMarkovModelStatePtr> _states;
  DiscreteIIDModelPtr _initial_probabilities;
  unsigned int _state_alphabet_size;
  unsigned int _observation_alphabet_size;
  unsigned int _max_backtracking;

 private:
  // Concrete methods
  Estimation<Labeling<Sequence>> viterbi(
      const Sequence &xs,
      Matrix &gamma,
      std::vector<EvaluatorPtr<Standard>> &observation_evaluators) const;

  Estimation<Labeling<Sequence>>
  posteriorDecoding(const Sequence &xs, Matrix &probabilities) const;

  std::vector<EvaluatorPtr<Standard>> initializeObservationEvaluators(
      const Sequence &xs, bool cached) const;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_
