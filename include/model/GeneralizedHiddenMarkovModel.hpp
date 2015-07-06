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
  using Cache = Base::Cache;

  // Hidden name method inheritance
  using Base::evaluateSequence;
  using Base::drawSequence;

  // Static methods
  static GeneralizedHiddenMarkovModelPtr make(
      std::vector<GeneralizedHiddenMarkovModelStatePtr> states,
      DiscreteIIDModelPtr initial_probabilities,
      unsigned int state_alphabet_size,
      unsigned int observation_alphabet_size,
      unsigned int max_backtracking = 100);

  // Overriden methods
  void initializeCache(CEPtr<Standard> evaluator,
                       unsigned int phase) override;

  Probability evaluateSymbol(SEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(SEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  Probability evaluateSymbol(CEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(CEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  void initializeCache(CEPtr<Labeling> evaluator,
                       unsigned int phase) override;

  Probability evaluateSymbol(SEPtr<Labeling> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(SEPtr<Labeling> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  Probability evaluateSymbol(CEPtr<Labeling> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(CEPtr<Labeling> evaluator,
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

  Estimation<Labeling<Sequence>>
  labeling(const Sequence &xs,
           Matrix &probabilities,
           Labeling<Sequence>::Method method) const override;

  // Virtual methods
  virtual double backward(const Sequence &s,
                          Matrix &beta) const;
  virtual double forward(const Sequence &s,
                         Matrix &alpha) const;
  virtual void posteriorProbabilities(const Sequence &xs,
                                      Matrix &probabilities) const;

  // Concrete methods
  // Estimation<Labeling<Sequence>>
  // simpleLabeling(SEPtr evaluator, Labeling<Sequence>::Method method);
  //
  // Estimation<Labeling<Sequence>>
  // cachedLabeling(CEPtr evaluator, Labeling<Sequence>::Method method);

 protected:
  // Instance variables
  std::vector<GeneralizedHiddenMarkovModelStatePtr> _states;
  DiscreteIIDModelPtr _initial_probabilities;
  unsigned int _state_alphabet_size;
  unsigned int _observation_alphabet_size;
  unsigned int _max_backtracking;

  // Constructors
  GeneralizedHiddenMarkovModel(
      std::vector<GeneralizedHiddenMarkovModelStatePtr> states,
      DiscreteIIDModelPtr initial_probability,
      unsigned int state_alphabet_size,
      unsigned int observation_alphabet_size,
      unsigned int max_backtracking);

 private:
  // Overriden methods
  Estimation<Labeling<Sequence>>
  viterbi(const Sequence &xs, Matrix &gamma) const override;

  Estimation<Labeling<Sequence>>
  posteriorDecoding(const Sequence &xs, Matrix &probabilities) const override;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_
