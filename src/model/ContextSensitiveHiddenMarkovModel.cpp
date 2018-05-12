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
#include "model/ContextSensitiveHiddenMarkovModel.hpp"

// Standard headers
#include <array>
#include <limits>
#include <memory>
#include <vector>
#include <utility>

// Macros
#define UNUSED(var) do { (void) sizeof(var); } while (false)

namespace tops {
namespace model {

// Aliases
using Counter = size_t;
using IID = DiscreteIIDModel;

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

ContextSensitiveHiddenMarkovModel::ContextSensitiveHiddenMarkovModel(
    std::vector<StatePtr> states,
    size_t state_alphabet_size,
    size_t observation_alphabet_size)
    : _states(std::move(states)),
      _state_alphabet_size(state_alphabet_size),
      _observation_alphabet_size(observation_alphabet_size) {
}

/*----------------------------------------------------------------------------*/
/*                              STATIC METHODS                                */
/*----------------------------------------------------------------------------*/

/*================================  TRAINER  =================================*/

ContextSensitiveHiddenMarkovModelPtr
ContextSensitiveHiddenMarkovModel::train(
    const TrainerPtr<Alignment, Self>& trainer,
    baum_welch_algorithm /* tag */,
    const ContextSensitiveHiddenMarkovModelPtr& initial_model,
    size_t max_iterations,
    Probability diff_threshold) {
  auto model
    = std::make_shared<ContextSensitiveHiddenMarkovModel>(*initial_model);

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
        for (const auto& state : model->states()) {
          for (auto s : state->successors()) {
            auto successor = model->state(s);

            if (!successor->hasGap(0) && i == sequences[0].size()) { continue; }

            A[state->id()][s] += alphas[state->id()][i]
              * state->transition()->probabilityOf(s)
              * successor->emission()->probabilityOf(
                  successor->hasGap(0) ? gap : sequences[0][i])
              * betas[s][i + successor->delta(0)]
              / full;
          }
        }
      }

      // Add contribution of the given sequences to matrix E
      for (size_t i = 0; i <= sequences[0].size(); i++) {
        for (const auto& state : model->states()) {
          if (!state->hasGap(0) && i == sequences[0].size()) { continue; }

          auto s0 = state->hasGap(0) ? gap : sequences[0][i];

          E[state->id()][s0]
            += alphas[state->id()][i + state->delta(0)]
            * betas[state->id()][i + state->delta(0)]
            / full;
        }
      }
    }

    // Replace states in the model
    for (auto& state : model->states()) {
      if (state->isSilent()) { continue; }

      auto k = state->id();

      // Replace the transition and emission of each state
      state->transition(IID::make(normalize<Expectation, 1>(A[k])));
      state->emission(IID::make(normalize<Expectation, 1>(E[k])));
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

ContextSensitiveHiddenMarkovModelPtr
ContextSensitiveHiddenMarkovModel::train(
    const TrainerPtr<Labeling, Self>& trainer,
    maximum_likelihood_algorithm /* tag */,
    const ContextSensitiveHiddenMarkovModelPtr& initial_model,
    size_t pseudo_counter) {
  auto model
    = std::make_shared<ContextSensitiveHiddenMarkovModel>(*initial_model);

  // Matrix for counting of emissions
  auto A = make_multiarray(pseudo_counter,
                           model->stateAlphabetSize(),
                           model->stateAlphabetSize());

  // Matrix for counting of transitions
  auto E = make_multiarray(pseudo_counter,
                           model->stateAlphabetSize(),
                           model->observationAlphabetSize());

  for (const auto& [ observation, other_observations, label ]
      : trainer->training_set()) {
    // Add contribution of the given sequences to matrix A
    for (size_t i = 0; i <= observation.size(); i++) {
      A[label[i]][label[i+1]] += 1;
    }

    // Add contribution of the given sequences to matrix E
    for (size_t i = 0; i < observation.size(); i++) {
      E[label[i+1]][observation[i]] += 1;
    }
  }

  for (auto& state : model->states()) {
    if (state->isSilent()) { continue; }

    auto k = state->id();

    // Replace the transition and emission of each state
    state->transition(IID::make(normalize<Counter, 1>(A[k])));
    state->emission(IID::make(normalize<Counter, 1>(E[k])));
  }

  return model;
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  SERIALIZER  ===============================*/

void ContextSensitiveHiddenMarkovModel::serialize(const SSPtr& serializer) {
  serializer->translator()->translate(this->shared_from_this());
}

/*=================================  OTHERS  =================================*/

typename ContextSensitiveHiddenMarkovModel::GeneratorReturn<Symbol>
ContextSensitiveHiddenMarkovModel::drawSymbol(
    const RandomNumberGeneratorPtr& rng,
    size_t pos,
    const Sequence& context) const {
  assert(!context.empty() && context[0] == _begin_id);

  Symbol label = _states[context[pos-1]]->transition()->draw(rng);
  Symbols alignment = { _states[label]->emission()->draw(rng) };

  return { label, alignment };
}

/*----------------------------------------------------------------------------*/

typename ContextSensitiveHiddenMarkovModel::GeneratorReturn<Sequence>
ContextSensitiveHiddenMarkovModel::drawSequence(
    const RandomNumberGeneratorPtr& rng,
    size_t size) const {
  Sequences alignment(1);
  Sequence label;

  label.push_back(_begin_id);
  for (size_t i = 1; i <= size; i++) {
    auto[ y, xs ] = drawSymbol(rng, i, label);

    // Keep trying to emit the right number of symbols
    if (y == _end_id) { i--; continue; }

    label.push_back(y);

    alignment[0].push_back(xs[0]);
  }
  label.push_back(_end_id);

  return { label, alignment };
}

/*================================  LABELER  =================================*/

typename ContextSensitiveHiddenMarkovModel::LabelerReturn
ContextSensitiveHiddenMarkovModel::viterbi(const Sequences& sequences) const {
  Probability zero;

  auto gammas = make_multiarray(
      zero, _state_alphabet_size, sequences[0].size() + 1);

  auto psi = make_multiarray(
      _begin_id, _state_alphabet_size, sequences[0].size() + 1);

  // Initialization
  gammas[_begin_id][0] = 1;

  // Recursion
  for (size_t i = 0; i <= sequences[0].size(); i++) {
    for (size_t j = 0; j <= sequences[0].size(); j++) {
      for (const auto& state : _states) {
        auto k = state->id();

        if (!state->hasGap(0) && i == 0) { continue; }

        for (auto p : state->predecessors()) {
          Probability candidate_max
            = gammas[p][i - state->delta(0)]
            * _states[p]->transition()->probabilityOf(state->id())
            * _states[k]->emission()->probabilityOf(
                state->hasGap(0) ? _gap : sequences[0][i-1]);

          if (candidate_max > gammas[state->id()][i]) {
            gammas[state->id()][i] = candidate_max;
            psi[state->id()][i] = p;
          }
        }
      }
    }
  }

  // Termination
  auto max = gammas[_end_id][sequences[0].size()];
  auto[ label, alignment ] = traceBack(sequences, psi);

  return { max, label, alignment, gammas };
}

/*----------------------------------------------------------------------------*/

typename ContextSensitiveHiddenMarkovModel::LabelerReturn
ContextSensitiveHiddenMarkovModel::posteriorDecoding(
    const Sequences& sequences) const {
  Probability zero;

  auto posteriors = make_multiarray(
      zero, _state_alphabet_size, sequences[0].size() + 1);

  auto psi = make_multiarray(
      _begin_id, _state_alphabet_size, sequences[0].size() + 1);

  // Preprocessment
  auto[ full, alphas ] = forward(sequences);
  auto[ _, betas ] = backward(sequences);

  UNUSED(_);  // A hack while we don't have pattern matching in C++

  // Initialization
  posteriors[_begin_id][0] = 1;

  // Recursion
  for (size_t i = 0; i <= sequences[0].size(); i++) {
    for (const auto& state : _states) {
      auto k = state->id();

      if (!state->hasGap(0) && i == 0) { continue; }

      for (auto p : state->predecessors()) {
        Probability candidate_max
          = posteriors[p][i - state->delta(0)]
          * (alphas[k][i] * betas[k][i] / full);

        if (candidate_max > posteriors[k][i]) {
          posteriors[k][i] = candidate_max;
          psi[k][i] = p;
        }
      }
    }
  }

  // Termination
  auto max = posteriors[_end_id][sequences[0].size()];
  auto[ label, alignment ] = traceBack(sequences, psi);

  return { max, label, alignment, posteriors };
}

/*----------------------------------------------------------------------------*/

typename ContextSensitiveHiddenMarkovModel::CalculatorReturn
ContextSensitiveHiddenMarkovModel::forward(const Sequences& sequences) const {
  Probability zero;

  auto alphas = make_multiarray(
      zero, _state_alphabet_size, sequences[0].size() + 1);

  // Initialization
  alphas[_begin_id][0] = 1;

  // Recursion
  for (size_t i = 0; i <= sequences[0].size(); i++) {
    for (const auto& state : _states) {
      auto k = state->id();

      if (!state->hasGap(0) && i == 0) { continue; }

      for (auto p : state->predecessors()) {
        alphas[state->id()][i]
          += alphas[p][i - state->delta(0)]
          * _states[p]->transition()->probabilityOf(k)
          * _states[k]->emission()->probabilityOf(
              state->hasGap(0) ? _gap : sequences[0][i-1]);
      }
    }
  }

  // Termination
  Probability full = alphas[_end_id][sequences[0].size()];

  return { full, alphas };
}

/*----------------------------------------------------------------------------*/

typename ContextSensitiveHiddenMarkovModel::CalculatorReturn
ContextSensitiveHiddenMarkovModel::backward(const Sequences& sequences) const {
  Probability zero;

  auto betas = make_multiarray(
      zero, _state_alphabet_size, sequences[0].size() + 2);

  // Initialization
  betas[_end_id][sequences[0].size()] = 1;

  // Recursion
  auto max = std::numeric_limits<size_t>::max();
  for (size_t i = sequences[0].size(); i != max; i--) {
    for (const auto& state : _states) {
      auto k = state->id();

      for (auto s : state->successors()) {
        if (!_states[s]->hasGap(0) && i == sequences[0].size()) { continue; }

        betas[state->id()][i]
          += betas[s][i + _states[s]->delta(0)]
          * _states[s]->emission()->probabilityOf(
              _states[s]->hasGap(0) ? _gap : sequences[0][i])
          * _states[k]->transition()->probabilityOf(s);
      }
    }
  }

  // Termination
  Probability full = betas[_begin_id][0];

  return { full, betas };
}

/*----------------------------------------------------------------------------*/

typename ContextSensitiveHiddenMarkovModel::TraceBackReturn
ContextSensitiveHiddenMarkovModel::traceBack(
    const Sequences& sequences,
    const MultiArray<typename State::Id, 2>& psi) const {
  Sequence label;
  Sequences alignment(1);

  // Initialization
  auto best_id = psi[_end_id][sequences[0].size()];

  std::vector<size_t> idxs { sequences[0].size() };

  // Iteration
  label.push_back(_end_id);
  while (best_id != _begin_id) {
    label.push_back(best_id);

    alignment[0].push_back(
        _states[best_id]->hasGap(0) ? _gap : sequences[0][idxs[0]-1]);

    best_id = psi[best_id][idxs[0]];

    if (!_states[best_id]->hasGap(0)) { idxs[0]--; }
  }
  label.push_back(_begin_id);

  // Termination
  std::reverse(label.begin(), label.end());
  std::reverse(alignment[0].begin(), alignment[0].end());

  return { label, alignment };
}

/*----------------------------------------------------------------------------*/

size_t ContextSensitiveHiddenMarkovModel::stateAlphabetSize() const {
  return _state_alphabet_size;
}

/*----------------------------------------------------------------------------*/

size_t ContextSensitiveHiddenMarkovModel::observationAlphabetSize() const {
  return _observation_alphabet_size;
}

/*----------------------------------------------------------------------------*/

auto ContextSensitiveHiddenMarkovModel::state(typename State::Id id)
    -> StatePtr {
  return _states[id];
}

/*----------------------------------------------------------------------------*/

auto ContextSensitiveHiddenMarkovModel::states() -> std::vector<StatePtr> {
  return _states;
}

/*----------------------------------------------------------------------------*/

auto ContextSensitiveHiddenMarkovModel::states() const
    -> const std::vector<StatePtr> {
  return _states;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
