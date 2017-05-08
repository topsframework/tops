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

// Interface header
#include "model/PairHiddenMarkovModel.hpp"

// Standard headers
#include <iomanip>
#include <utility>
#include <iostream>

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

PairHiddenMarkovModel::PairHiddenMarkovModel(
    std::vector<StatePtr> states,
    std::size_t state_alphabet_size,
    std::size_t observation_alphabet_size)
    : _states(std::move(states)),
      _state_alphabet_size(std::move(state_alphabet_size)),
      _observation_alphabet_size(std::move(observation_alphabet_size)) {
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

Estimation<Labeling<Sequences>>
PairHiddenMarkovModel::viterbi(const Sequences& sequences, Cube& gammas) const {
  Symbol gap = _observation_alphabet_size;

  typename State::Id begin_id = 0;
  typename State::Id end_id = _state_alphabet_size-1;

  auto psi = std::vector<std::vector<std::vector<unsigned int>>>(_state_alphabet_size,
      std::vector<std::vector<unsigned int>>(sequences[0].size() + 2,
        std::vector<unsigned int>(sequences[1].size() + 2, begin_id)));

  gammas = Cube(_state_alphabet_size,
      Matrix(sequences[0].size() + 2,
        std::vector<Probability>(sequences[1].size() + 2)));

  // Initialization
  for (const auto& state : _states) {
    if (state->isSilent()) continue;

    gammas[state->id()][1][1]
      = _states[begin_id]->transition()->probabilityOf(state->id())
      * state->emission()->probabilityOf(
          state->hasGap(1) ? gap : sequences[0][0],
          state->hasGap(2) ? gap : sequences[1][0]);
  }

  // Recursion
  for (unsigned int i = 1; i <= sequences[0].size(); i++) {
    for (unsigned int j = 1; j <= sequences[1].size(); j++) {
      if (i == 1 && j == 1) continue;

      for (const auto& state : _states) {
        if (state->isSilent()) continue;

        Probability max_probability;
        typename State::Id max_id = begin_id;

        for(auto p : state->predecessors()) {
          Probability v = gammas[p][i - state->delta(1)][j - state->delta(2)]
            * _states[p]->transition()->probabilityOf(state->id());

          if (v > max_probability) {
             max_probability = v;
             max_id = p;
          }
        }

        gammas[state->id()][i][j] = max_probability;
        psi[state->id()][i][j] = max_id;

        gammas[state->id()][i][j]
          *= state->emission()->probabilityOf(
              state->hasGap(1) ? gap : sequences[0][i-1],
              state->hasGap(2) ? gap : sequences[1][j-1]);
      }
    }
  }

  // Termination
  Probability max_probability;
  typename State::Id max_id = begin_id;
  for (auto p : _states[end_id]->predecessors()) {
    Probability v = gammas[p][sequences[0].size()][sequences[1].size()]
      * _states[p]->transition()->probabilityOf(end_id);

    if (v > max_probability) {
      max_probability = v;
      max_id = p;
    }
  }

  // Trace back
  std::vector<std::size_t> idxs {
    0, sequences[0].size(), sequences[1].size() };

  Sequence path, al1, al2;
  typename State::Id best_id = max_id;

  path.push_back(end_id);
  while (best_id != begin_id) {
    path.push_back(best_id);

    al1.push_back(_states[best_id]->hasGap(1) ? gap : sequences[0][idxs[1]-1]);
    al2.push_back(_states[best_id]->hasGap(2) ? gap : sequences[1][idxs[2]-1]);

    best_id = psi[best_id][idxs[1]][idxs[2]];

    if (!_states[best_id]->hasGap(1)) idxs[1]--;
    if (!_states[best_id]->hasGap(2)) idxs[2]--;
  }
  path.push_back(begin_id);

  std::reverse(path.begin(), path.end());
  std::reverse(al1.begin(), al1.end());
  std::reverse(al2.begin(), al2.end());

  return Estimation<Labeling<Sequences>>(
      Labeling<Sequences>(Sequences{ al1, al2 }, path), max_probability);
}

/*----------------------------------------------------------------------------*/

Estimation<Labeling<Sequences>>
PairHiddenMarkovModel::posteriorDecoding(const Sequences& sequences,
                                         Cube& probabilities) const {
  Symbol gap = _observation_alphabet_size;

  typename State::Id begin_id = 0;
  typename State::Id end_id = _state_alphabet_size-1;

  auto psi = std::vector<std::vector<std::vector<unsigned int>>>(_state_alphabet_size,
      std::vector<std::vector<unsigned int>>(sequences[0].size() + 2,
        std::vector<unsigned int>(sequences[1].size() + 2, begin_id)));

  // Initialization
  posteriorProbabilities(sequences, probabilities);

  // Recursion
  for (unsigned int i = 1; i <= sequences[0].size(); i++) {
    for (unsigned int j = 1; j <= sequences[1].size(); j++) {
      if (i == 1 && j == 1) continue;

      for (const auto& state : _states) {
        if (state->isSilent()) continue;

        Probability max_probability;
        typename State::Id max_id = begin_id;

        for(auto p : state->predecessors()) {
          Probability v = probabilities[p][i - state->delta(1)][j - state->delta(2)]
            * probabilities[state->id()][i][j];

          if (v > max_probability) {
            max_probability = v;
            max_id = p;
          }
        }

        probabilities[state->id()][i][j] = max_probability;
        psi[state->id()][i][j] = max_id;
      }
    }
  }

  // Termination
  Probability max_probability;
  typename State::Id max_id = begin_id;
  for (auto p : _states[end_id]->predecessors()) {
    Probability v = probabilities[p][sequences[0].size()][sequences[1].size()];

    if (v > max_probability) {
      max_probability = v;
      max_id = p;
    }
  }

  // Trace back
  std::vector<std::size_t> idxs {
    0, sequences[0].size(), sequences[1].size() };

  Sequence path, al1, al2;
  typename State::Id best_id = max_id;

  path.push_back(end_id);
  while (best_id != begin_id) {
    path.push_back(best_id);

    al1.push_back(_states[best_id]->hasGap(1) ? gap : sequences[0][idxs[1]-1]);
    al2.push_back(_states[best_id]->hasGap(2) ? gap : sequences[1][idxs[2]-1]);

    best_id = psi[best_id][idxs[1]][idxs[2]];

    if (!_states[best_id]->hasGap(1)) idxs[1]--;
    if (!_states[best_id]->hasGap(2)) idxs[2]--;
  }
  path.push_back(begin_id);

  std::reverse(path.begin(), path.end());
  std::reverse(al1.begin(), al1.end());
  std::reverse(al2.begin(), al2.end());

  return Estimation<Labeling<Sequences>>(
      Labeling<Sequences>(Sequences{ al1, al2 }, path), max_probability);
}

/*----------------------------------------------------------------------------*/

Probability PairHiddenMarkovModel::forward(const Sequences& sequences,
                                           Cube& alphas) const {
  Symbol gap = _observation_alphabet_size;

  typename State::Id begin_id = 0;
  typename State::Id end_id = _state_alphabet_size-1;

  alphas = Cube(_state_alphabet_size,
      Matrix(sequences[0].size() + 2,
        std::vector<Probability>(sequences[1].size() + 2)));

  // Initialization
  for (const auto& state : _states) {
    if (state->isSilent()) continue;

    alphas[state->id()][1][1]
      = _states[begin_id]->transition()->probabilityOf(state->id())
      * state->emission()->probabilityOf(
          state->hasGap(1) ? gap : sequences[0][0],
          state->hasGap(2) ? gap : sequences[1][0]);
  }

  // Recursion
  for (unsigned int i = 1; i <= sequences[0].size(); i++) {
    for (unsigned int j = 1; j <= sequences[1].size(); j++) {
      if (i == 1 && j == 1) continue;

      for (const auto& state : _states) {
        if (state->isSilent()) continue;

        for(auto p : state->predecessors()) {
          alphas[state->id()][i][j]
            += alphas[p][i - state->delta(1)][j - state->delta(2)]
            * _states[p]->transition()->probabilityOf(state->id());
        }

        alphas[state->id()][i][j]
          *= state->emission()->probabilityOf(
              state->hasGap(1) ? gap : sequences[0][i-1],
              state->hasGap(2) ? gap : sequences[1][j-1]);
      }
    }
  }

  // Termination
  Probability sum;
  for (auto p : _states[end_id]->predecessors())
    sum += alphas[p][sequences[0].size()][sequences[1].size()]
      * _states[p]->transition()->probabilityOf(end_id);

  return sum;
}

/*----------------------------------------------------------------------------*/

Probability PairHiddenMarkovModel::backward(const Sequences& sequences,
                                            Cube& betas) const {
  Symbol gap = _observation_alphabet_size;

  typename State::Id begin_id = 0;
  typename State::Id end_id = _state_alphabet_size-1;

  betas = Cube(_state_alphabet_size,
      Matrix(sequences[0].size() + 2,
        std::vector<Probability>(sequences[1].size() + 2)));

  // Initialization
  for (const auto& state : _states) {
    if (state->isSilent()) continue;

    betas[state->id()][sequences[0].size()][sequences[1].size()]
      = state->transition()->probabilityOf(end_id);
  }

  // Recursion
  for (unsigned int i = sequences[0].size(); i >= 1; i--) {
    for (unsigned int j = sequences[1].size(); j >= 1; j--) {
      if (i == sequences[0].size() && j == sequences[1].size()) continue;

      for (const auto& state : _states) {
        if (state->isSilent()) continue;

        for(auto s : state->successors()) {
          betas[state->id()][i][j]
            += betas[s][i + _states[s]->delta(1)][j + _states[s]->delta(2)]
            * _states[s]->emission()->probabilityOf(
                _states[s]->hasGap(1) ? gap : sequences[0][i-1],
                _states[s]->hasGap(2) ? gap : sequences[1][j-1])
            * state->transition()->probabilityOf(s);
        }
      }
    }
  }

  // Termination
  Probability sum;
  for (auto s : _states[begin_id]->successors()) {
    if (_states[s]->isSilent()) continue;

    sum += betas[s][1][1]
        * _states[s]->emission()->probabilityOf(
            _states[s]->hasGap(1) ? gap : sequences[0][0],
            _states[s]->hasGap(2) ? gap : sequences[1][0])
        * _states[begin_id]->transition()->probabilityOf(s);
  }

  return sum;
}

/*----------------------------------------------------------------------------*/

void PairHiddenMarkovModel::posteriorProbabilities(const Sequences& sequences,
                                                   Cube& probabilities) const {
  probabilities = Cube(_state_alphabet_size,
      Matrix(sequences[0].size() + 2,
        std::vector<Probability>(sequences[1].size() + 2)));

  // Initialization
  Cube alphas;  // forward
  Cube betas;   // backward

  Probability full = forward(sequences, alphas);
  backward(sequences, betas);

  // Calculation
  for (unsigned int i = 1; i <= sequences[0].size(); i++) {
    for (unsigned int j = 1; j <= sequences[1].size(); j++) {
      for (const auto& state : _states) {
        if (state->isSilent()) continue;

        probabilities[state->id()][i][j]
          = (alphas[state->id()][i][j] * betas[state->id()][i][j]) / full;
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

std::size_t PairHiddenMarkovModel::stateAlphabetSize() const {
  return _state_alphabet_size;
}

/*----------------------------------------------------------------------------*/

std::size_t PairHiddenMarkovModel::observationAlphabetSize() const {
  return _observation_alphabet_size;
}

/*----------------------------------------------------------------------------*/

auto PairHiddenMarkovModel::state(typename State::Id id) -> StatePtr {
  return _states[id];
}

/*----------------------------------------------------------------------------*/

auto PairHiddenMarkovModel::states() -> std::vector<StatePtr> {
  return _states;
}

/*----------------------------------------------------------------------------*/

auto PairHiddenMarkovModel::states() const -> const std::vector<StatePtr> {
  return _states;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
