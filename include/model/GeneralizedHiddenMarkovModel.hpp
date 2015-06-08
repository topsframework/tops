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
#include "model/DecodableEvaluator.hpp"
#include "model/GeneralizedHiddenMarkovModelState.hpp"
#include "model/GeneralizedHiddenMarkovModelSignalState.hpp"
#include "model/GeneralizedHiddenMarkovModelExplicitDurationState.hpp"

// ToPS templates
#include "model/Labeling.tcc"
#include "model/Estimation.tcc"
#include "model/DecodableModelCrtp.tcc"
#include "model/SimpleEvaluatorImpl.tcc"

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

  using Cache = DecodableModel::Cache;
  using SEPtr = SimpleEvaluatorImplPtr<GeneralizedHiddenMarkovModel>;
  using CEPtr = CachedEvaluatorImplPtr<GeneralizedHiddenMarkovModel>;

  // Static methods
  static GeneralizedHiddenMarkovModelPtr make(
      std::vector<GeneralizedHiddenMarkovModelStatePtr> states,
      DiscreteIIDModelPtr initial_probabilities,
      unsigned int state_alphabet_size,
      unsigned int observation_alphabet_size,
      unsigned int max_backtracking = 100);

  // Overriden methods
  Labeling<Sequence> simpleChoose(SGPtr<Labeling<Sequence>> generator,
                                  unsigned int size,
                                  unsigned int phase = 0) const override;

  Estimation<Labeling<Sequence>>
  labeling(const Sequence &xs,
           Matrix &probabilities,
           Labeling<Sequence>::Method method) const override;

  // Virtual methods
  virtual EvaluatorPtr evaluator(const Sequence &s,
                                 bool cached = false);
  virtual DecodableEvaluatorPtr decodableEvaluator(const Sequence &s,
                                                   bool cached = false);

  virtual double evaluate(const Sequence &s,
                          unsigned int pos,
                          unsigned int phase = 0) const;
  virtual double evaluate(const Sequence &xs,
                          const Sequence &ys,
                          unsigned int i) const;

  virtual Symbol choose(const Sequence &xs,
                        unsigned int i,
                        unsigned int phase = 0) const;
  virtual void chooseSequencesPosition(Sequence &xs,
                                       Sequence &ys,
                                       unsigned int i) const;

  virtual double backward(const Sequence &s,
                          Matrix &beta) const;
  virtual double forward(const Sequence &s,
                         Matrix &alpha) const;
  virtual void posteriorProbabilities(const Sequence &xs,
                                      Matrix &probabilities) const;

  // Concrete methods
  void initializeCachedEvaluator(CEPtr evaluator,
                                 unsigned int phase = 0);

  double simpleProbabilityOf(SEPtr evaluator,
                             unsigned int begin,
                             unsigned int end,
                             unsigned int phase = 0) const;
  double cachedProbabilityOf(CEPtr evaluator,
                             unsigned int begin,
                             unsigned int end,
                             unsigned int phase = 0) const;

  double simpleProbabilityOf(SEPtr evaluator,
                             const Sequence& s,
                             unsigned int begin,
                             unsigned int end) const;

  Estimation<Labeling<Sequence>>
  simpleLabeling(SEPtr evaluator, Labeling<Sequence>::Method method);

  Estimation<Labeling<Sequence>>
  cachedLabeling(CEPtr evaluator, Labeling<Sequence>::Method method);

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
