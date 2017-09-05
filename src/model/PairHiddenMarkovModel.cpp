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

typename PairHiddenMarkovModel::GeneratorReturn<Symbol>
PairHiddenMarkovModel::drawSymbol(RandomNumberGeneratorPtr rng,
                                  std::size_t pos,
                                  const Sequence& context) const {
  assert(context.size() > 0 && context[0] == _begin_id);

  Symbol label = _states[context[pos-1]]->transition()->draw(rng);
  Symbols alignment = _states[label]->emission()->drawPair(rng);

  return { label, alignment };
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::GeneratorReturn<Sequence>
PairHiddenMarkovModel::drawSequence(RandomNumberGeneratorPtr rng,
                                    std::size_t size) const {
  Sequences alignment(2);
  Sequence label;

  label.push_back(_begin_id);
  for (std::size_t i = 1; i <= size; i++) {
    auto [ y, xs ] = drawSymbol(rng, i, label);

    // Keep trying to emit the right number of symbols
    if (y == _end_id) { i--; continue; }

    label.push_back(y);

    alignment[0].push_back(xs[0]);
    alignment[1].push_back(xs[1]);
  }
  label.push_back(_end_id);

  return { label, alignment };
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::LabelerReturn
PairHiddenMarkovModel::viterbi(const Sequences& sequences) const {
  auto psi = MultiArray<typename State::Id, 3>(_state_alphabet_size,
      MultiArray<typename State::Id, 2>(sequences[0].size() + 1,
        MultiArray<typename State::Id, 1>(sequences[1].size() + 1, _begin_id)));

  auto gammas = Cube(_state_alphabet_size,
      Matrix(sequences[0].size() + 1,
        std::vector<Probability>(sequences[1].size() + 1)));

  // Initialization
  gammas[_begin_id][0][0] = 1;

  // Recursion
  for (std::size_t i = 0; i <= sequences[0].size(); i++) {
    for (std::size_t j = 0; j <= sequences[1].size(); j++) {
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
  auto max = gammas[_end_id][sequences[0].size()][sequences[1].size()];
  auto [ label, alignment ] = traceBack(sequences, psi);

  return { max, label, alignment, gammas };
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::LabelerReturn
PairHiddenMarkovModel::posteriorDecoding(const Sequences& sequences) const {
  auto psi = MultiArray<typename State::Id, 3>(_state_alphabet_size,
      MultiArray<typename State::Id, 2>(sequences[0].size() + 1,
        MultiArray<typename State::Id, 1>(sequences[1].size() + 1, _begin_id)));

  auto posteriors = Cube(_state_alphabet_size,
      Matrix(sequences[0].size() + 1,
        std::vector<Probability>(sequences[1].size() + 1)));

  // Preprocessment
  auto [ full, alphas ] = forward(sequences);
  auto [ _, betas ] = backward(sequences);

  UNUSED(_);  // A hack while we don't have pattern matching in C++

  // Initialization
  posteriors[_begin_id][0][0] = 1;

  // Recursion
  for (std::size_t i = 0; i <= sequences[0].size(); i++) {
    for (std::size_t j = 0; j <= sequences[1].size(); j++) {
      for (const auto& state : _states) {
        if (!state->hasGap(0) && i == 0) continue;
        if (!state->hasGap(1) && j == 0) continue;

        for(auto p : state->predecessors()) {
          Probability v
            = posteriors[p][i - state->delta(0)][j - state->delta(1)];

          if (v > posteriors[state->id()][i][j]) {
            posteriors[state->id()][i][j] = v;
            psi[state->id()][i][j] = p;
          }
        }

        posteriors[state->id()][i][j]
          *= ((alphas[state->id()][i][j] * betas[state->id()][i][j]) / full);
      }
    }
  }

  // Termination
  auto max = posteriors[_end_id][sequences[0].size()][sequences[1].size()];
  auto [ label, alignment ] = traceBack(sequences, psi);

  return { max, label, alignment, posteriors };
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::CalculatorReturn
PairHiddenMarkovModel::forward(const Sequences& sequences) const {
  auto alphas = Cube(_state_alphabet_size,
      Matrix(sequences[0].size() + 1,
        std::vector<Probability>(sequences[1].size() + 1)));

  // Initialization
  alphas[_begin_id][0][0] = 1;

  // Recursion
  for (std::size_t i = 0; i <= sequences[0].size(); i++) {
    for (std::size_t j = 0; j <= sequences[1].size(); j++) {
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
  betas[_end_id][sequences[0].size()][sequences[1].size()] = 1;

  // Recursion
  auto max = std::numeric_limits<std::size_t>::max();
  for (std::size_t i = sequences[0].size(); i != max; i--) {
    for (std::size_t j = sequences[1].size(); j != max; j--) {
      for (const auto& state : _states) {
        for(auto s : state->successors()) {
          if (!_states[s]->hasGap(0) && i == sequences[0].size()) continue;
          if (!_states[s]->hasGap(1) && j == sequences[1].size()) continue;

          betas[state->id()][i][j]
            += betas[s][i + _states[s]->delta(0)][j + _states[s]->delta(1)]
            * _states[s]->emission()->probabilityOf(
                _states[s]->hasGap(0) ? _gap : sequences[0][i],
                _states[s]->hasGap(1) ? _gap : sequences[1][j])
            * state->transition()->probabilityOf(s);
        }
      }
    }
  }

  // Termination
  Probability full = betas[_begin_id][0][0];

  return { full, betas };
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::TraceBackReturn
PairHiddenMarkovModel::traceBack(
    const Sequences& sequences,
    const MultiArray<typename State::Id, 3>& psi) const {
  Sequence label;
  Sequences alignment(2);

  // Initialization
  auto best_id = psi[_end_id][sequences[0].size()][sequences[1].size()];

  std::vector<std::size_t> idxs { sequences[0].size(), sequences[1].size() };

  // Iteration
  label.push_back(_end_id);
  while (best_id != _begin_id) {
    label.push_back(best_id);

    alignment[0].push_back(
        _states[best_id]->hasGap(0) ? _gap : sequences[0][idxs[0]-1]);
    alignment[1].push_back(
        _states[best_id]->hasGap(1) ? _gap : sequences[1][idxs[1]-1]);

    best_id = psi[best_id][idxs[0]][idxs[1]];

    if (!_states[best_id]->hasGap(0)) idxs[0]--;
    if (!_states[best_id]->hasGap(1)) idxs[1]--;
  }
  label.push_back(_begin_id);

  // Termination
  std::reverse(label.begin(), label.end());
  std::reverse(alignment[0].begin(), alignment[0].end());
  std::reverse(alignment[1].begin(), alignment[1].end());

  return { label, alignment };
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
