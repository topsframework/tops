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
#include <limits>
#include <memory>
#include <vector>
#include <utility>

// Macros
#define UNUSED(var) do { (void) sizeof(var); } while (false)

namespace tops {
namespace model {

// Aliases
using IID = DiscreteIIDModel;

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

PairHiddenMarkovModel::PairHiddenMarkovModel(
    std::vector<StatePtr> states,
    size_t state_alphabet_size,
    size_t observation_alphabet_size)
    : _states(std::move(states)),
      _state_alphabet_size(state_alphabet_size),
      _observation_alphabet_size(observation_alphabet_size) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

/*================================  TRAINER  =================================*/

PairHiddenMarkovModelPtr
PairHiddenMarkovModel::train(const TrainerPtr<Alignment, Self>& trainer,
                             baum_welch_algorithm /* tag */,
                             const PairHiddenMarkovModelPtr& initial_model,
                             size_t max_iterations,
                             Probability diff_threshold) {
  auto model = std::make_shared<PairHiddenMarkovModel>(*initial_model);

  Symbol gap = model->observationAlphabetSize();

  std::pair<Expectation, Expectation> lasts;
  for (size_t iteration = 0; iteration < max_iterations; iteration++) {
    Expectation zero;

    // Matrix for expectations of transitions
    auto A = make_multiarray(zero,
                             model->stateAlphabetSize(),
                             model->stateAlphabetSize());

    // Matrix for expectations of emissions
    auto E = make_multiarray(zero,
                             model->stateAlphabetSize(),
                             model->observationAlphabetSize()+1,
                             model->observationAlphabetSize()+1);

    Expectation last;
    for (const auto& sequences : trainer->training_set()) {
      // Forward and backward values
      auto[ full, alphas ] = model->forward(sequences);
      auto[ _, betas ] = model->backward(sequences);

      UNUSED(_);  // A hack while we don't have pattern matching in C++

      last += full;

      // Add contribution of the given sequences to matrix A
      for (size_t i = 0; i <= sequences[0].size(); i++) {
        for (size_t j = 0; j <= sequences[1].size(); j++) {
          for (const auto& state : model->states()) {
            for (auto s : state->successors()) {
              auto successor = model->state(s);

              if (!successor->hasGap(0) && i == sequences[0].size()) {
                continue;
              }
              if (!successor->hasGap(1) && j == sequences[1].size()) {
                continue;
              }

              A[state->id()][s] += alphas[state->id()][i][j]
                * state->transition()->probabilityOf(s)
                * successor->emission()->probabilityOf(
                    successor->hasGap(0) ? gap : sequences[0][i],
                    successor->hasGap(1) ? gap : sequences[1][j])
                * betas[s][i + successor->delta(0)][j + successor->delta(1)]
                / full;
            }
          }
        }
      }

      // Add contribution of the given sequences to matrix E
      for (size_t i = 0; i <= sequences[0].size(); i++) {
        for (size_t j = 0; j <= sequences[1].size(); j++) {
          for (const auto& state : model->states()) {
            if (!state->hasGap(0) && i == sequences[0].size()) { continue; }
            if (!state->hasGap(1) && j == sequences[1].size()) { continue; }

            auto s0 = state->hasGap(0) ? gap : sequences[0][i];
            auto s1 = state->hasGap(1) ? gap : sequences[1][j];

            E[state->id()][s0][s1]
              += alphas[state->id()][i + state->delta(0)][j + state->delta(1)]
               * betas[state->id()][i + state->delta(0)][j + state->delta(1)]
               / full;
          }
        }
      }
    }

    // Replace states in the model
    for (auto& state : model->states()) {
      if (state->isSilent()) { continue; }

      auto k = state->id();

      // Replace the transition and emission of each state
      state->transition(IID::make(normalize<Expectation, 1>(A[k])));
      state->emission(IID::make(normalize<Expectation, 2>(E[k])));
    }

    // Store last expectancies of alignment
    lasts = std::make_pair(last, lasts.first);

    // Difference between expectancies
    Probability diff = lasts.second > lasts.first ?
      lasts.second - lasts.first : lasts.first - lasts.second;

    // Finish if expectancies do not change
    if (diff < diff_threshold) { break; }
  }

  return model;
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  SERIALIZER  ===============================*/

void PairHiddenMarkovModel::serialize(const SSPtr& serializer) {
  serializer->translator()->translate(this->shared_from_this());
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::GeneratorReturn<Symbol>
PairHiddenMarkovModel::drawSymbol(const RandomNumberGeneratorPtr& rng,
                                  size_t pos,
                                  const Sequence& context) const {
  assert(!context.empty() && context[0] == _begin_id);

  Symbol label = _states[context[pos-1]]->transition()->draw(rng);
  Symbols alignment = _states[label]->emission()->drawPair(rng);

  return { label, alignment };
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::GeneratorReturn<Sequence>
PairHiddenMarkovModel::drawSequence(const RandomNumberGeneratorPtr& rng,
                                    size_t size) const {
  Sequences alignment(2);
  Sequence label;

  label.push_back(_begin_id);
  for (size_t i = 1; i <= size; i++) {
    auto[ y, xs ] = drawSymbol(rng, i, label);

    // Keep trying to emit the right number of symbols
    if (y == _end_id) { i--; continue; }

    label.push_back(y);

    alignment[0].push_back(xs[0]);
    alignment[1].push_back(xs[1]);
  }
  label.push_back(_end_id);

  return { label, alignment };
}

/*================================  LABELER  =================================*/

typename PairHiddenMarkovModel::LabelerReturn
PairHiddenMarkovModel::viterbi(const Sequences& sequences) const {
  Probability zero;

  auto gammas = make_multiarray(zero,
                                _state_alphabet_size,
                                sequences[0].size() + 1,
                                sequences[1].size() + 1);

  auto psi = make_multiarray(_begin_id,
                             _state_alphabet_size,
                             sequences[0].size() + 1,
                             sequences[1].size() + 1);

  // Initialization
  gammas[_begin_id][0][0] = 1;

  // Recursion
  for (size_t i = 0; i <= sequences[0].size(); i++) {
    for (size_t j = 0; j <= sequences[1].size(); j++) {
      for (const auto& state : _states) {
        auto k = state->id();

        if (!state->hasGap(0) && i == 0) { continue; }
        if (!state->hasGap(1) && j == 0) { continue; }

        for (auto p : state->predecessors()) {
          Probability candidate_max
            = gammas[p][i - state->delta(0)][j - state->delta(1)]
            * _states[p]->transition()->probabilityOf(state->id())
            * _states[k]->emission()->probabilityOf(
                state->hasGap(0) ? _gap : sequences[0][i-1],
                state->hasGap(1) ? _gap : sequences[1][j-1]);

          if (candidate_max > gammas[state->id()][i][j]) {
             gammas[state->id()][i][j] = candidate_max;
             psi[state->id()][i][j] = p;
          }
        }
      }
    }
  }

  // Termination
  auto max = gammas[_end_id][sequences[0].size()][sequences[1].size()];
  auto[ label, alignment ] = traceBack(sequences, psi);

  return { max, label, alignment, gammas };
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::LabelerReturn
PairHiddenMarkovModel::posteriorDecoding(const Sequences& sequences) const {
  Probability zero;

  auto posteriors = make_multiarray(zero,
                                    _state_alphabet_size,
                                    sequences[0].size() + 1,
                                    sequences[1].size() + 1);

  auto psi = make_multiarray(_begin_id,
                             _state_alphabet_size,
                             sequences[0].size() + 1,
                             sequences[1].size() + 1);

  // Preprocessment
  auto[ full, alphas ] = forward(sequences);
  auto[ _, betas ] = backward(sequences);

  UNUSED(_);  // A hack while we don't have pattern matching in C++

  // Initialization
  posteriors[_begin_id][0][0] = 1;

  // Recursion
  for (size_t i = 0; i <= sequences[0].size(); i++) {
    for (size_t j = 0; j <= sequences[1].size(); j++) {
      for (const auto& state : _states) {
        auto k = state->id();

        if (!state->hasGap(0) && i == 0) { continue; }
        if (!state->hasGap(1) && j == 0) { continue; }

        for (auto p : state->predecessors()) {
          Probability candidate_max
            = posteriors[p][i - state->delta(0)][j - state->delta(1)]
            * (alphas[k][i][j] * betas[k][i][j] / full);

          if (candidate_max > posteriors[k][i][j]) {
            posteriors[k][i][j] = candidate_max;
            psi[k][i][j] = p;
          }
        }
      }
    }
  }

  // Termination
  auto max = posteriors[_end_id][sequences[0].size()][sequences[1].size()];
  auto[ label, alignment ] = traceBack(sequences, psi);

  return { max, label, alignment, posteriors };
}

/*----------------------------------------------------------------------------*/

typename PairHiddenMarkovModel::CalculatorReturn
PairHiddenMarkovModel::forward(const Sequences& sequences) const {
  Probability zero;

  auto alphas = make_multiarray(zero,
                                _state_alphabet_size,
                                sequences[0].size() + 1,
                                sequences[1].size() + 1);

  // Initialization
  alphas[_begin_id][0][0] = 1;

  // Recursion
  for (size_t i = 0; i <= sequences[0].size(); i++) {
    for (size_t j = 0; j <= sequences[1].size(); j++) {
      for (const auto& state : _states) {
        auto k = state->id();

        if (!state->hasGap(0) && i == 0) { continue; }
        if (!state->hasGap(1) && j == 0) { continue; }

        for (auto p : state->predecessors()) {
          alphas[k][i][j]
            += alphas[p][i - state->delta(0)][j - state->delta(1)]
            * _states[p]->transition()->probabilityOf(state->id())
            * _states[k]->emission()->probabilityOf(
               state->hasGap(0) ? _gap : sequences[0][i-1],
               state->hasGap(1) ? _gap : sequences[1][j-1]);
        }
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
  Probability zero;

  auto betas = make_multiarray(zero,
                               _state_alphabet_size,
                               sequences[0].size() + 2,
                               sequences[1].size() + 2);

  // Initialization
  betas[_end_id][sequences[0].size()][sequences[1].size()] = 1;

  // Recursion
  auto max = std::numeric_limits<size_t>::max();
  for (size_t i = sequences[0].size(); i != max; i--) {
    for (size_t j = sequences[1].size(); j != max; j--) {
      for (const auto& state : _states) {
        auto k = state->id();

        for (auto s : state->successors()) {
          if (!_states[s]->hasGap(0) && i == sequences[0].size()) { continue; }
          if (!_states[s]->hasGap(1) && j == sequences[1].size()) { continue; }

          betas[k][i][j]
            += betas[s][i + _states[s]->delta(0)][j + _states[s]->delta(1)]
            * _states[s]->emission()->probabilityOf(
                _states[s]->hasGap(0) ? _gap : sequences[0][i],
                _states[s]->hasGap(1) ? _gap : sequences[1][j])
            * _states[k]->transition()->probabilityOf(s);
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

  std::vector<size_t> idxs { sequences[0].size(), sequences[1].size() };

  // Iteration
  label.push_back(_end_id);
  while (best_id != _begin_id) {
    label.push_back(best_id);

    alignment[0].push_back(
        _states[best_id]->hasGap(0) ? _gap : sequences[0][idxs[0]-1]);
    alignment[1].push_back(
        _states[best_id]->hasGap(1) ? _gap : sequences[1][idxs[1]-1]);

    best_id = psi[best_id][idxs[0]][idxs[1]];

    if (!_states[best_id]->hasGap(0)) { idxs[0]--; }
    if (!_states[best_id]->hasGap(1)) { idxs[1]--; }
  }
  label.push_back(_begin_id);

  // Termination
  std::reverse(label.begin(), label.end());
  std::reverse(alignment[0].begin(), alignment[0].end());
  std::reverse(alignment[1].begin(), alignment[1].end());

  return { label, alignment };
}

/*----------------------------------------------------------------------------*/

size_t PairHiddenMarkovModel::stateAlphabetSize() const {
  return _state_alphabet_size;
}

/*----------------------------------------------------------------------------*/

size_t PairHiddenMarkovModel::observationAlphabetSize() const {
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
