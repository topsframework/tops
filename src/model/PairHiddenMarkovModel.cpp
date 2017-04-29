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
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size)
    : _states(std::move(states)),
      _state_alphabet_size(std::move(state_alphabet_size)),
      _observation_alphabet_size(std::move(observation_alphabet_size)) {
}

/*----------------------------------------------------------------------------*/

Estimation<Labeling<Alignment>>
PairHiddenMarkovModel::viterbi(const Alignment& sequences, Cube& gammas) const {
  Symbol gap = _observation_alphabet_size;

  typename State::Id begin_id = 0;
  typename State::Id end_id = _state_alphabet_size-1;

  auto psi = std::vector<std::vector<std::vector<unsigned int>>>(_state_alphabet_size,
      std::vector<std::vector<unsigned int>>(sequences.first.size() + 2,
        std::vector<unsigned int>(sequences.second.size() + 2, begin_id)));

  gammas = Cube(_state_alphabet_size,
      Matrix(sequences.first.size() + 2,
        std::vector<Probability>(sequences.second.size() + 2)));

  // Initialization
  for (const auto& state : _states) {
    if (state->isSilent()) continue;

    gammas[state->id()][1][1]
      = _states[begin_id]->transition()->probabilityOf(state->id())
      * state->emission()->probabilityOf(
          state->hasGap(1) ? gap : sequences.first[0],
          state->hasGap(2) ? gap : sequences.second[0]);
  }

  // Recursion
  for (unsigned int i = 1; i <= sequences.first.size(); i++) {
    for (unsigned int j = 1; j <= sequences.second.size(); j++) {
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
              state->hasGap(1) ? gap : sequences.first[i-1],
              state->hasGap(2) ? gap : sequences.second[j-1]);
      }
    }
  }

  // Termination
  Probability max_probability;
  typename State::Id max_id = begin_id;
  for (auto p : _states[end_id]->predecessors()) {
    Probability v = gammas[p][sequences.first.size()][sequences.second.size()]
      * _states[p]->transition()->probabilityOf(end_id);

    if (v > max_probability) {
      max_probability = v;
      max_id = p;
    }
  }

  // Trace back
  std::vector<std::size_t> idxs {
    0, sequences.first.size(), sequences.second.size() };

  Sequence path, al1, al2;
  typename State::Id best_id = max_id;

  path.push_back(end_id);
  while (best_id != begin_id) {
    path.push_back(best_id);

    al1.push_back(_states[best_id]->hasGap(1) ? gap : sequences.first[idxs[1]-1]);
    al2.push_back(_states[best_id]->hasGap(2) ? gap : sequences.second[idxs[2]-1]);

    best_id = psi[best_id][idxs[1]][idxs[2]];

    if (!_states[best_id]->hasGap(1)) idxs[1]--;
    if (!_states[best_id]->hasGap(2)) idxs[2]--;
  }
  path.push_back(begin_id);

  std::reverse(path.begin(), path.end());
  std::reverse(al1.begin(), al1.end());
  std::reverse(al2.begin(), al2.end());

  return Estimation<Labeling<Alignment>>(
      Labeling<Alignment>(Alignment{ al1, al2 }, path), max_probability);
}

/*----------------------------------------------------------------------------*/

Estimation<Labeling<Alignment>>
PairHiddenMarkovModel::posteriorDecoding(const Alignment& sequences,
                                         Cube& probabilities) const {
  Symbol gap = _observation_alphabet_size;

  typename State::Id begin_id = 0;
  typename State::Id end_id = _state_alphabet_size-1;

  auto psi = std::vector<std::vector<std::vector<unsigned int>>>(_state_alphabet_size,
      std::vector<std::vector<unsigned int>>(sequences.first.size() + 2,
        std::vector<unsigned int>(sequences.second.size() + 2, begin_id)));

  // Initialization
  posteriorProbabilities(sequences, probabilities);

  // Recursion
  for (unsigned int i = 1; i <= sequences.first.size(); i++) {
    for (unsigned int j = 1; j <= sequences.second.size(); j++) {
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
    Probability v = probabilities[p][sequences.first.size()][sequences.second.size()];

    if (v > max_probability) {
      max_probability = v;
      max_id = p;
    }
  }

  // Trace back
  std::vector<std::size_t> idxs {
    0, sequences.first.size(), sequences.second.size() };

  Sequence path, al1, al2;
  typename State::Id best_id = max_id;

  path.push_back(end_id);
  while (best_id != begin_id) {
    path.push_back(best_id);

    al1.push_back(_states[best_id]->hasGap(1) ? gap : sequences.first[idxs[1]-1]);
    al2.push_back(_states[best_id]->hasGap(2) ? gap : sequences.second[idxs[2]-1]);

    best_id = psi[best_id][idxs[1]][idxs[2]];

    if (!_states[best_id]->hasGap(1)) idxs[1]--;
    if (!_states[best_id]->hasGap(2)) idxs[2]--;
  }
  path.push_back(begin_id);

  std::reverse(path.begin(), path.end());
  std::reverse(al1.begin(), al1.end());
  std::reverse(al2.begin(), al2.end());

  return Estimation<Labeling<Alignment>>(
      Labeling<Alignment>(Alignment{ al1, al2 }, path), max_probability);
}

/*----------------------------------------------------------------------------*/

Probability PairHiddenMarkovModel::forward(const Alignment& sequences,
                                           Cube& alphas) const {
  Symbol gap = _observation_alphabet_size;

  typename State::Id begin_id = 0;
  typename State::Id end_id = _state_alphabet_size-1;

  alphas = Cube(_state_alphabet_size,
      Matrix(sequences.first.size() + 2,
        std::vector<Probability>(sequences.second.size() + 2)));

  // Initialization
  for (const auto& state : _states) {
    if (state->isSilent()) continue;

    alphas[state->id()][1][1]
      = _states[begin_id]->transition()->probabilityOf(state->id())
      * state->emission()->probabilityOf(
          state->hasGap(1) ? gap : sequences.first[0],
          state->hasGap(2) ? gap : sequences.second[0]);
  }

  // Recursion
  for (unsigned int i = 1; i <= sequences.first.size(); i++) {
    for (unsigned int j = 1; j <= sequences.second.size(); j++) {
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
              state->hasGap(1) ? gap : sequences.first[i-1],
              state->hasGap(2) ? gap : sequences.second[j-1]);
      }
    }
  }

  // Termination
  Probability sum;
  for (auto p : _states[end_id]->predecessors())
    sum += alphas[p][sequences.first.size()][sequences.second.size()]
      * _states[p]->transition()->probabilityOf(end_id);

  return sum;
}

/*----------------------------------------------------------------------------*/

Probability PairHiddenMarkovModel::backward(const Alignment& sequences,
                                            Cube& betas) const {
  Symbol gap = _observation_alphabet_size;

  typename State::Id begin_id = 0;
  typename State::Id end_id = _state_alphabet_size-1;

  betas = Cube(_state_alphabet_size,
      Matrix(sequences.first.size() + 2,
        std::vector<Probability>(sequences.second.size() + 2)));

  // Initialization
  for (const auto& state : _states) {
    if (state->isSilent()) continue;

    betas[state->id()][sequences.first.size()][sequences.second.size()]
      = state->transition()->probabilityOf(end_id);
  }

  // Recursion
  for (unsigned int i = sequences.first.size(); i >= 1; i--) {
    for (unsigned int j = sequences.second.size(); j >= 1; j--) {
      if (i == sequences.first.size() && j == sequences.second.size()) continue;

      for (const auto& state : _states) {
        if (state->isSilent()) continue;

        for(auto s : state->successors()) {
          betas[state->id()][i][j]
            += betas[s][i + _states[s]->delta(1)][j + _states[s]->delta(2)]
            * _states[s]->emission()->probabilityOf(
                _states[s]->hasGap(1) ? gap : sequences.first[i-1],
                _states[s]->hasGap(2) ? gap : sequences.second[j-1])
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
            _states[s]->hasGap(1) ? gap : sequences.first[0],
            _states[s]->hasGap(2) ? gap : sequences.second[0])
        * _states[begin_id]->transition()->probabilityOf(s);
  }

  return sum;
}

/*----------------------------------------------------------------------------*/

void PairHiddenMarkovModel::posteriorProbabilities(const Alignment& sequences,
                                                   Cube& probabilities) const {
  probabilities = Cube(_state_alphabet_size,
      Matrix(sequences.first.size() + 2,
        std::vector<Probability>(sequences.second.size() + 2)));

  // Initialization
  Cube alphas;  // forward
  Cube betas;   // backward

  Probability full = forward(sequences, alphas);
  backward(sequences, betas);

  // Calculation
  for (unsigned int i = 1; i <= sequences.first.size(); i++) {
    for (unsigned int j = 1; j <= sequences.second.size(); j++) {
      for (const auto& state : _states) {
        if (state->isSilent()) continue;

        probabilities[state->id()][i][j]
          = (alphas[state->id()][i][j] * betas[state->id()][i][j]) / full;
      }
    }
  }
}

}  // namespace model
}  // namespace tops
