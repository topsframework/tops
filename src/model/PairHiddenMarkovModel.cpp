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

typename PairHiddenMarkovModel::LabelerReturn
PairHiddenMarkovModel::viterbi(const Sequences& sequences) const {
  auto psi = MultiArray<typename State::Id, 3>(_state_alphabet_size,
      MultiArray<typename State::Id, 2>(sequences[0].size() + 2,
        MultiArray<typename State::Id, 1>(sequences[1].size() + 2, _begin_id)));

  auto gammas = Cube(_state_alphabet_size,
      Matrix(sequences[0].size() + 2,
        std::vector<Probability>(sequences[1].size() + 2)));

  // Initialization
  gammas[_begin_id][0][0] = 1;

  // Recursion
  for (unsigned int i = 0; i <= sequences[0].size(); i++) {
    for (unsigned int j = 0; j <= sequences[1].size(); j++) {
      for (const auto& state : _states) {
        if (!state->hasGap(0) && i == 0) continue;
        if (!state->hasGap(1) && j == 0) continue;

        for(auto p : state->predecessors()) {
          Probability v = gammas[p][i - state->delta(0)][j - state->delta(1)]
            * _states[p]->transition()->probabilityOf(state->id());

          if (v > gammas[state->id()][i][j]) {
             gammas[state->id()][i][j] = v;
             psi[state->id()][i][j] = p;
          }
        }

        gammas[state->id()][i][j]
          *= state->emission()->probabilityOf(
              state->hasGap(0) ? _gap : sequences[0][i-1],
              state->hasGap(1) ? _gap : sequences[1][j-1]);
      }
    }
  }

  // Termination
  Probability max_probability = gammas[_end_id][sequences[0].size()][sequences[1].size()];
  typename State::Id max_id = psi[_end_id][sequences[0].size()][sequences[1].size()];

  // Trace back
  std::vector<std::size_t> idxs {
    sequences[0].size(), sequences[1].size() };

  Sequence path, al1, al2;
  typename State::Id best_id = max_id;

  path.push_back(_end_id);
  while (best_id != _begin_id) {
    path.push_back(best_id);

    al1.push_back(_states[best_id]->hasGap(0) ? _gap : sequences[0][idxs[0]-1]);
    al2.push_back(_states[best_id]->hasGap(1) ? _gap : sequences[1][idxs[1]-1]);

    best_id = psi[best_id][idxs[0]][idxs[1]];

    if (!_states[best_id]->hasGap(0)) idxs[0]--;
    if (!_states[best_id]->hasGap(1)) idxs[1]--;
  }
  path.push_back(_begin_id);

  std::reverse(path.begin(), path.end());
  std::reverse(al1.begin(), al1.end());
  std::reverse(al2.begin(), al2.end());

  return { max_probability, path, Sequences{ al1, al2 }, gammas };
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::LabelerReturn
PairHiddenMarkovModel::posteriorDecoding(const Sequences& sequences) const {
  auto psi = MultiArray<typename State::Id, 3>(_state_alphabet_size,
      MultiArray<typename State::Id, 2>(sequences[0].size() + 2,
        MultiArray<typename State::Id, 1>(sequences[1].size() + 2, _begin_id)));

  auto probabilities = Cube(_state_alphabet_size,
      Matrix(sequences[0].size() + 2,
        std::vector<Probability>(sequences[1].size() + 2)));

	// Preprocessment
  auto [ full, alphas ] = forward(sequences);
  auto [ _, betas ] = backward(sequences);

  // Initialization
  probabilities[_begin_id][0][0] = 1;

  // Recursion
  for (unsigned int i = 0; i <= sequences[0].size(); i++) {
    for (unsigned int j = 0; j <= sequences[1].size(); j++) {
      for (const auto& state : _states) {
        if (!state->hasGap(0) && i == 0) continue;
        if (!state->hasGap(1) && j == 0) continue;

        for(auto p : state->predecessors()) {
          Probability v
            = probabilities[p][i - state->delta(0)][j - state->delta(1)];

          if (v > probabilities[state->id()][i][j]) {
            probabilities[state->id()][i][j] = v;
            psi[state->id()][i][j] = p;
          }
        }

				if (!state->isSilent())
					probabilities[state->id()][i][j]
						*= ((alphas[state->id()][i][j] * betas[state->id()][i][j]) / full);
      }
    }
  }

  // Termination
  Probability max_probability = probabilities[_end_id][sequences[0].size()][sequences[1].size()];
  typename State::Id max_id = psi[_end_id][sequences[0].size()][sequences[1].size()];

  // Trace back
  std::vector<std::size_t> idxs {
    sequences[0].size(), sequences[1].size() };

  Sequence path, al1, al2;
  typename State::Id best_id = max_id;

  path.push_back(_end_id);
  while (best_id != _begin_id) {
    path.push_back(best_id);

    al1.push_back(_states[best_id]->hasGap(0) ? _gap : sequences[0][idxs[0]-1]);
    al2.push_back(_states[best_id]->hasGap(1) ? _gap : sequences[1][idxs[1]-1]);

    best_id = psi[best_id][idxs[0]][idxs[1]];

    if (!_states[best_id]->hasGap(0)) idxs[0]--;
    if (!_states[best_id]->hasGap(1)) idxs[1]--;
  }
  path.push_back(_begin_id);

  std::reverse(path.begin(), path.end());
  std::reverse(al1.begin(), al1.end());
  std::reverse(al2.begin(), al2.end());

  return { max_probability, path, Sequences{ al1, al2 }, probabilities };
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::CalculatorReturn
PairHiddenMarkovModel::forward(const Sequences& sequences) const {
  auto alphas = Cube(_state_alphabet_size,
      Matrix(sequences[0].size() + 2,
        std::vector<Probability>(sequences[1].size() + 2)));

  // Initialization
  alphas[_begin_id][0][0] = 1;

  // Recursion
  for (unsigned int i = 0; i <= sequences[0].size(); i++) {
    for (unsigned int j = 0; j <= sequences[1].size(); j++) {
      for (const auto& state : _states) {
        if (!state->hasGap(0) && i == 0) continue;
        if (!state->hasGap(1) && j == 0) continue;

        for (auto p : state->predecessors()) {
          alphas[state->id()][i][j]
            += alphas[p][i - state->delta(0)][j - state->delta(1)]
            * _states[p]->transition()->probabilityOf(state->id());
        }

        alphas[state->id()][i][j]
          *= state->emission()->probabilityOf(
              state->hasGap(0) ? _gap : sequences[0][i-1],
              state->hasGap(1) ? _gap : sequences[1][j-1]);
      }
    }
  }

  // Termination
  Probability full = alphas[_end_id][sequences[0].size()][sequences[1].size()];

  return { full, alphas };
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::CalculatorReturn
PairHiddenMarkovModel::backward(const Sequences& sequences) const {
  auto betas = Cube(_state_alphabet_size,
      Matrix(sequences[0].size() + 2,
        std::vector<Probability>(sequences[1].size() + 2)));

  // Initialization
  betas[_end_id][sequences[0].size()+1][sequences[1].size()+1] = 1;

  // Recursion
  for (unsigned int i = sequences[0].size()+1; i >= 1; i--) {
    for (unsigned int j = sequences[1].size()+1; j >= 1; j--) {
      for (const auto& state : _states) {
        for(auto s : state->successors()) {
          if (!_states[s]->hasGap(0) && i == sequences[0].size()+1) continue;
          if (!_states[s]->hasGap(1) && j == sequences[1].size()+1) continue;

          betas[state->id()][i][j]
            += betas[s][i + _states[s]->delta(0)][j + _states[s]->delta(1)]
            * _states[s]->emission()->probabilityOf(
                _states[s]->hasGap(0) ? _gap : sequences[0][i-1],
                _states[s]->hasGap(1) ? _gap : sequences[1][j-1])
            * state->transition()->probabilityOf(s);
        }
      }
    }
  }

  // Termination
  Probability full = betas[_begin_id][1][1];

  return { full, betas };
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
