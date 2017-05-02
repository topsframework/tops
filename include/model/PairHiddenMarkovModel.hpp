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
#include "model/Matrix.hpp"
#include "model/Labeling.hpp"
#include "model/Sequence.hpp"
#include "model/Estimation.hpp"
#include "model/Probability.hpp"
#include "model/SimpleState.hpp"
#include "model/DiscreteIIDModel.hpp"
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
class PairHiddenMarkovModel {
 public:
  // Aliases
  using Self = PairHiddenMarkovModel;
  using SelfPtr = PairHiddenMarkovModelPtr;
  using Base = void;
  // using Cache = Base::Cache;

  // Type traits
  using State = typename StateTraits<Self>::State;
  using StatePtr = std::shared_ptr<State>;

  using MatchState  = typename StateTraits<Self>::MatchState;
  using Gap1State   = typename StateTraits<Self>::Gap1State;
  using Gap2State   = typename StateTraits<Self>::Gap2State;
  using SilentState = typename StateTraits<Self>::SilentState;

  /*=============================[ CONSTRUCTORS ]=============================*/

  PairHiddenMarkovModel(std::vector<StatePtr> states,
                        std::size_t state_alphabet_size,
                        std::size_t observation_alphabet_size);

  /*==========================[ CONCRETE METHODS ]============================*/

  // Labeler's helpers
  Estimation<Labeling<Alignment>>
  viterbi(const Alignment& sequences, Cube& gammas) const;

  Estimation<Labeling<Alignment>>
  posteriorDecoding(const Alignment& sequences, Cube& probabilities) const;

  // Calculator's helpers
  Probability forward(const Alignment& sequences, Cube& alphas) const;
  Probability backward(const Alignment& sequences, Cube& betas) const;

  // Others
  void posteriorProbabilities(const Alignment& sequences,
                              Cube& probabilities) const;

 protected:
  // Instance variables
  std::vector<StatePtr> _states;
  std::size_t _state_alphabet_size;
  std::size_t _observation_alphabet_size;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PAIR_HIDDEN_MARKOV_MODEL_
