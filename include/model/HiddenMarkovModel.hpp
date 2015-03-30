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
#include "model/DecodableModel.hpp"
#include "model/HiddenMarkovModelState.hpp"
#include "model/Matrix.hpp"
#include "model/Labeling.hpp"

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
class HiddenMarkovModel : public DecodableModel {
 public:
  static HiddenMarkovModelPtr make(
      std::vector<HiddenMarkovModelStatePtr> states,
      DiscreteIIDModelPtr initial_probabilities,
      unsigned int state_alphabet_size,
      unsigned int observation_alphabet_size);

  static HiddenMarkovModelPtr trainML(std::vector<Sequence> observation_training_set,
                                      std::vector<Sequence> state_training_set,
                                      unsigned int state_alphabet_size,
                                      unsigned int observation_alphabet_size,
                                      double pseudocont);

  static HiddenMarkovModelPtr trainBaumWelch(std::vector<Sequence> observation_training_set,
                                      HiddenMarkovModelPtr initial_model,
                                      unsigned int maxiterations,
                                      double diff_threshold);

  virtual double evaluatePosition(const Sequence &xs, unsigned int i, unsigned int phase = 0) const;
  virtual Symbol choosePosition(const Sequence &xs, unsigned int i, unsigned int phase = 0) const;

  virtual double evaluateSequences(const Sequence &xs, const Sequence &ys, unsigned int begin, unsigned int end) const;
  virtual double evaluateSequencesPosition(const Sequence &xs, const Sequence &ys, unsigned int i) const;
  virtual void chooseSequences(Sequence &xs, Sequence &ys, unsigned int size) const;
  virtual void chooseSequencesPosition(Sequence &xs, Sequence &ys, unsigned int i) const;

  virtual double backward(const Sequence & s, Matrix &beta) const;
  virtual double forward(const Sequence & s, Matrix &alpha) const;

  virtual void posteriorProbabilities(const Sequence & xs, Matrix & probabilities) const;

  unsigned int stateAlphabetSize() const;
  unsigned int observationAlphabetSize() const;
  HiddenMarkovModelStatePtr state(unsigned int i) const;

  virtual Labeling labeling(const Sequence &xs, Matrix &probabilities, 
                            Labeling::Method method) const override;

  virtual Labeling viterbi(const Sequence &xs, Matrix &gamma) const override;
  virtual Labeling posteriorDecoding(const Sequence &xs, Matrix &probabilities) const override;

 private:
  HiddenMarkovModel(
      std::vector<HiddenMarkovModelStatePtr> states,
      DiscreteIIDModelPtr initial_probability,
      unsigned int state_alphabet_size,
      unsigned int observation_alphabet_size);

  std::vector<HiddenMarkovModelStatePtr> _states;
    DiscreteIIDModelPtr _initial_probabilities;
    unsigned int _state_alphabet_size;
    unsigned int _observation_alphabet_size;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_HIDDEN_MARKOV_MODEL_
