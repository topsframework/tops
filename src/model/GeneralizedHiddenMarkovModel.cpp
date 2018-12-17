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
#include "model/GeneralizedHiddenMarkovModel.hpp"

// Standard headers
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

// Macros
#define UNUSED(var) do { (void) sizeof(var); } while (false)

namespace tops {
namespace model {

// Aliases
using Counter = std::size_t;
using IID = DiscreteIIDModel;

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

GeneralizedHiddenMarkovModel::GeneralizedHiddenMarkovModel(
    std::vector<StatePtr> states,
    std::size_t state_alphabet_size,
    std::size_t observation_alphabet_size,
    std::size_t num_phases,
    std::size_t max_backtracking)
    : _states(std::move(states)),
      _state_alphabet_size(state_alphabet_size),
      _observation_alphabet_size(observation_alphabet_size),
      _num_phases(num_phases),
      _max_backtracking(max_backtracking) {
}

/*----------------------------------------------------------------------------*/
/*                              STATIC METHODS                                */
/*----------------------------------------------------------------------------*/

/*================================  TRAINER  =================================*/

GeneralizedHiddenMarkovModelPtr
GeneralizedHiddenMarkovModel::train(
    const TrainerPtr<Labeling, Self>& trainer,
    maximum_likelihood_algorithm /* tag */,
    const GeneralizedHiddenMarkovModelPtr& initial_model,
    std::size_t pseudo_counter) {
  auto model = std::make_shared<GeneralizedHiddenMarkovModel>(*initial_model);

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
    for (std::size_t i = 0; i <= observation.size(); i++) {
      A[label[i]][label[i+1]] += 1;
    }

    // Add contribution of the given sequences to matrix E
    for (std::size_t i = 0; i < observation.size(); i++) {
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

void GeneralizedHiddenMarkovModel::serialize(const SSPtr& serializer) {
  serializer->translator()->translate(this->shared_from_this());
}

/*=================================  OTHERS  =================================*/

typename GeneralizedHiddenMarkovModel::GeneratorReturn<Symbol>
GeneralizedHiddenMarkovModel::drawSymbol(const RandomNumberGeneratorPtr& rng,
                                         std::size_t pos,
                                         const Sequence& context) const {
  assert(!context.empty() && context[0] == _begin_id);

  // TODO(renatocf): Implement drawing of duration
  Symbol label = _states[context[pos-1]]->transition()->draw(rng);

  Symbols alignment = {
    _states[label]->emission()->standardGenerator(rng)->drawSymbol(pos) };

  return { label, alignment };
}

/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::GeneratorReturn<Sequence>
GeneralizedHiddenMarkovModel::drawSequence(const RandomNumberGeneratorPtr& rng,
                                           std::size_t size) const {
  Sequences alignment(1);
  Sequence label;

  label.push_back(_begin_id);
  for (std::size_t i = 1; i <= size; i++) {
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

typename GeneralizedHiddenMarkovModel::LabelerReturn
GeneralizedHiddenMarkovModel::viterbi(
    const Sequences& sequences,
    const Matrix&  extrinsic_probabilities ) const {
  Probability zero;

  auto gammas = make_multiarray(
      zero,
      sequences[0].size() + 1,
      _state_alphabet_size);

  auto phi = make_multiarray(
      _begin_id,
      sequences[0].size() + 1,
      _state_alphabet_size);

  auto psi = make_multiarray(
      _begin_id,
      sequences[0].size() + 1,
      _state_alphabet_size);

  // Initialization
  gammas[0][_begin_id] = 1;

  // Recursion
  for (std::size_t i = 0; i <= sequences[0].size(); i++) {
    for (const auto& state : _states) {
      auto k = state->id();

      if (!state->hasGap(0) && i == 0) { continue; }

      auto max_length = std::min(i, _max_backtracking);
      for (auto d : _states[k]->duration()->possibleLengths(max_length)) {
        auto begin = i - d - state->beginExtension();
        auto end = i + state->endExtension();
        auto phase =
          (state->beginPhase() - state->beginExtension()) % _num_phases;

        // Verify extesion
        bool exceedsBegin = (begin < state->beginExtension());
        bool exceedsEnd = (end + state->endExtension() > sequences[0].size());
        if (exceedsBegin || exceedsEnd) { break; }

        // Duration is valid
        Probability duration_probability =
          state->duration()->probabilityOfLenght(d);

        // TODO(renatocf): segment_probability should be lazy!
        Probability segment_probability =
          state->emission()
               ->standardEvaluator(sequences[0])
               ->evaluateSequence(begin, end, phase);

        // Code should be here if caches are working
        // If model is locally factorable, we stop the backtracking
        if (segment_probability == zero) { break; }

        Probability extrinsic_contribuition = 1;
        for (auto ii = begin; ii < end; ii++) {
          extrinsic_contribuition *= extrinsic_probabilities[k][ii];
        }

        for (auto p : state->predecessors()) {
          // No need to compute if predecessor has probability 0
          if (gammas[i - d][p] == zero) { continue; }

          Probability transition_probability =
            _states[p]->transition()->probabilityOf(k);

          Probability candidate_max
            = gammas[i - d][p]
            * transition_probability
            * duration_probability
            * segment_probability
            * extrinsic_contribuition;

          if (candidate_max > gammas[i][k]) {
            gammas[i][k] = candidate_max;
            phi[i][k] = d;
            psi[i][k] = p;
          }
        }
      }
    }
  }

  // Termination
  auto max = gammas[sequences[0].size()][_end_id];
  auto[ label, alignment ] = traceBack(sequences, psi, phi);

  return { max, label, alignment, gammas };
}

/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::LabelerReturn
GeneralizedHiddenMarkovModel::posteriorDecoding(
    const Sequences& sequences) const {
  const Probability zero;

  auto posteriors = make_multiarray(
      zero,
      sequences[0].size() + 1,
      _state_alphabet_size);

  auto phi = make_multiarray(
      _begin_id,
      sequences[0].size() + 1,
      _state_alphabet_size);

  auto psi = make_multiarray(
      _begin_id,
      sequences[0].size() + 1,
      _state_alphabet_size);

  // Preprocessment
  auto[ full, alphas ] = forward(sequences);
  auto[ _, betas ] = backward(sequences);

  UNUSED(_);  // A hack while we don't have pattern matching in C++

  // Initialization
  posteriors[0][_begin_id] = 1;

  // Recursion
  for (std::size_t i = 0; i <= sequences[0].size(); i++) {
    for (const auto& state : _states) {
      auto k = state->id();

      if (!state->hasGap(0) && i == 0) { continue; }

      for (auto p : state->predecessors()) {
        Probability candidate_max
          = posteriors[i - state->delta(0)][p]
          * ((alphas[i][k] * betas[i][k]) / full);

        if (candidate_max > posteriors[i][k]) {
          posteriors[i][k] = candidate_max;
          phi[i][k] = 1;
          psi[i][k] = p;
        }
      }
    }
  }

  // Termination
  auto max = posteriors[sequences[0].size()][_end_id];
  auto[ label, alignment ] = traceBack(sequences, psi, phi);

  return { max, label, alignment, posteriors };
}

/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::CalculatorReturn
GeneralizedHiddenMarkovModel::forward(const Sequences& sequences) const {
  const Probability zero;

  auto alphas = make_multiarray(
      zero,
      sequences[0].size() + 1,
      _state_alphabet_size);

  // Initialization
  alphas[0][_begin_id] = 1;

  // Recursion
  for (std::size_t i = 0; i <= sequences[0].size(); i++) {
    for (const auto& state : _states) {
      auto k = state->id();
      auto phase = state->beginPhase();

      if (!state->hasGap(0) && i == 0) { continue; }

      auto max_length = std::min(i, _max_backtracking);
      for (auto d : _states[k]->duration()->possibleLengths(max_length)) {
        auto begin = i-d, end = i;

        if (!segmentIsViable(sequences[0], begin, end, _states[k])) {
          continue;
        }

        for (auto p : state->predecessors()) {
          alphas[i][k]
            += alphas[i - d][p]
            * _states[p]->transition()
                        ->probabilityOf(k)
            * _states[k]->duration()
                        ->probabilityOfLenght(d)
            * _states[k]->emission()
                        ->standardEvaluator(sequences[0])
                        ->evaluateSequence(begin, end, phase);
        }
      }
    }
  }

  // Termination
  Probability full = alphas[sequences[0].size()][_end_id];

  return { full, alphas };
}

/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::CalculatorReturn
GeneralizedHiddenMarkovModel::backward(const Sequences& sequences) const {
  const Probability zero;

  auto betas = make_multiarray(
      zero,
      sequences[0].size() + 2,
      _state_alphabet_size);

  // Initialization
  betas[sequences[0].size()][_end_id] = 1;

  // Recursion
  auto max = std::numeric_limits<std::size_t>::max();
  for (std::size_t i = sequences[0].size(); i != max; i--) {
    for (const auto& state : _states) {
      auto k = state->id();

      for (auto s : state->successors()) {
        if (!_states[s]->hasGap(0) && i == sequences[0].size()) { continue; }

        auto phase = _states[s]->beginPhase();

        auto max_length = std::min(sequences[0].size()-i, _max_backtracking);
        for (auto d : _states[s]->duration()->possibleLengths(max_length)) {
          auto begin = i, end = i+d;

          if (!segmentIsViable(sequences[0], begin, end, _states[s])) {
            continue;
          }

          betas[i][k]
            += _states[k]->transition()
                         ->probabilityOf(s)
            * _states[s]->duration()
                        ->probabilityOfLenght(d)
            * _states[s]->emission()
                        ->standardEvaluator(sequences[0])
                        ->evaluateSequence(begin, end, phase)
            * betas[i + d][s];
        }
      }
    }
  }

  // Termination
  Probability full = betas[0][_begin_id];

  return { full, betas };
}

/*----------------------------------------------------------------------------*/

bool GeneralizedHiddenMarkovModel::segmentIsViable(
    const Sequence& sequence,
    std::size_t begin,
    std::size_t end,
    const StatePtr& state) const {
  const Probability zero;

  bool exceedsBegin = (begin < state->beginExtension());
  bool exceedsEnd = (end + state->endExtension() > sequence.size());

  if (exceedsBegin || exceedsEnd) { return false; }

  auto extended_begin
    = begin - state->beginExtension();
  auto extended_end
    = end + state->endExtension();
  auto extended_phase
    = (state->beginPhase() - state->beginExtension()) % _num_phases;

  auto prob = state->emission()
                   ->standardEvaluator(sequence)
                   ->evaluateSequence(extended_begin,
                                      extended_end,
                                      extended_phase);

  return prob != zero;
}

/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::TraceBackReturn
GeneralizedHiddenMarkovModel::traceBack(
    const Sequences& sequences,
    const MultiArray<typename State::Id, 2>& psi,
    const MultiArray<typename std::size_t, 2>& phi) const {
  Sequence label;
  Sequences alignment(1);

  // Initialization
  std::vector<std::size_t> idxs { sequences[0].size() };

  auto best_id = psi[idxs[0]][_end_id];

  // Iteration
  label.push_back(_end_id);
  while (best_id != _begin_id) {
    auto duration = phi[idxs[0]][best_id];

    label.insert(label.end(), duration, best_id);

    if (_states[best_id]->hasGap(0)) {
      alignment[0].insert(alignment[0].end(), duration, _gap);
    } else {
      alignment[0].insert(
          alignment[0].end(),
          sequences[0].begin() + idxs[0] - duration,
          sequences[0].begin() + idxs[0]);
    }

    best_id = psi[idxs[0]][best_id];

    if (!_states[best_id]->hasGap(0)) { idxs[0] -= duration; }
  }
  label.push_back(_begin_id);

  // Termination
  std::reverse(label.begin(), label.end());
  std::reverse(alignment[0].begin(), alignment[0].end());

  return { label, alignment };
}

/*----------------------------------------------------------------------------*/

std::size_t GeneralizedHiddenMarkovModel::stateAlphabetSize() const {
  return _state_alphabet_size;
}

/*----------------------------------------------------------------------------*/

std::size_t GeneralizedHiddenMarkovModel::observationAlphabetSize() const {
  return _observation_alphabet_size;
}

/*----------------------------------------------------------------------------*/

auto GeneralizedHiddenMarkovModel::state(typename State::Id id) -> StatePtr {
  return _states[id];
}

/*----------------------------------------------------------------------------*/

auto GeneralizedHiddenMarkovModel::states() -> std::vector<StatePtr> {
  return _states;
}

/*----------------------------------------------------------------------------*/

auto GeneralizedHiddenMarkovModel::states() const
    -> const std::vector<StatePtr> {
  return _states;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
