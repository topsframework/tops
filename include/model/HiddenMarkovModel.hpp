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

// ToPS headers
#include "model/Matrix.hpp"
#include "model/HiddenMarkovModelState.hpp"

// ToPS templates
#include "model/DecodableModelCrtp.tcc"

namespace tops {
namespace model {

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
  // Alias
  using Base = DecodableModelCrtp<HiddenMarkovModel>;
  using Cache = Base::Cache;

  // Hidden name method inheritance
  using Base::evaluateSequence;
  using Base::drawSequence;

  // Static methods
  static HiddenMarkovModelPtr make(
      std::vector<HiddenMarkovModelStatePtr> states,
      DiscreteIIDModelPtr initial_probabilities,
      unsigned int state_alphabet_size,
      unsigned int observation_alphabet_size);

  static HiddenMarkovModelPtr trainML(
      std::vector<Sequence> observation_training_set,
      std::vector<Sequence> state_training_set,
      unsigned int state_alphabet_size,
      unsigned int observation_alphabet_size,
      double pseudocont);

  static HiddenMarkovModelPtr trainBaumWelch(
      std::vector<Sequence> observation_training_set,
      HiddenMarkovModelPtr initial_model,
      unsigned int maxiterations,
      double diff_threshold);

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

  unsigned int stateAlphabetSize() const;
  unsigned int observationAlphabetSize() const;
  HiddenMarkovModelStatePtr state(unsigned int i) const;

 protected:
  // Instance variables
  std::vector<HiddenMarkovModelStatePtr> _states;
  DiscreteIIDModelPtr _initial_probabilities;
  unsigned int _state_alphabet_size;
  unsigned int _observation_alphabet_size;

  // Constructors
  HiddenMarkovModel(std::vector<HiddenMarkovModelStatePtr> states,
                    DiscreteIIDModelPtr initial_probability,
                    unsigned int state_alphabet_size,
                    unsigned int observation_alphabet_size);

 private:
  // Overriden methods
  Estimation<Labeling<Sequence>>
  viterbi(const Sequence &xs, Matrix &gamma) const override;

  Estimation<Labeling<Sequence>>
  posteriorDecoding(const Sequence &xs, Matrix &probabilities) const override;

  // Concrete methods
  void initializeCache(const Sequence &sequence, Cache &cache);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_HIDDEN_MARKOV_MODEL_
