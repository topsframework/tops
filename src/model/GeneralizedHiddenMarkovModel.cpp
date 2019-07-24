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
using Counter = size_t;
using IID = DiscreteIIDModel;

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

GeneralizedHiddenMarkovModel::GeneralizedHiddenMarkovModel(
    std::vector<StatePtr> states,
    size_t state_alphabet_size,
    size_t observation_alphabet_size,
    size_t num_phases,
    size_t max_backtracking)
    : DecodableModelCrtp(
        std::move(states),
        state_alphabet_size,
        observation_alphabet_size),
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
    size_t pseudo_counter) {
  auto model = std::make_shared<GeneralizedHiddenMarkovModel>(*initial_model);

  // Matrix for counting of emissions
  auto A = make_multiarray(pseudo_counter,
                           model->stateAlphabetSize(),
                           model->stateAlphabetSize());

  // Matrix for counting of transitions
  auto E = make_multiarray(pseudo_counter,
                           model->stateAlphabetSize(),
                           model->observationAlphabetSize());

  for (const auto& [ observations, label ] : trainer->training_set()) {
    // Add contribution of the given sequences to matrix A
    for (size_t i = 0; i <= observations[0].size(); i++) {
      A[label[i]][label[i+1]] += 1;
    }

    // Add contribution of the given sequences to matrix E
    for (size_t i = 0; i < observations[0].size(); i++) {
      E[label[i+1]][observations[0][i]] += 1;
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
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

Probability
GeneralizedHiddenMarkovModel::evaluateSymbol(SEPtr<Multiple> evaluator,
                                             size_t pos,
                                             size_t /* phase */) const {
  auto[ full, alphas ] = forward({ evaluator->sequence() });
  return alphas[0][pos];
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSequence(SEPtr<Multiple> evaluator,
                                               size_t begin,
                                               size_t end,
                                               size_t phase) const {
  return Base::evaluateSequence(evaluator, begin, end, phase);
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSymbol(SEPtr<Labeling> evaluator,
                                  size_t pos,
                                  size_t /* phase */) const {
  auto[ full, alphas ] = forward(evaluator->sequence().observations);
  return alphas[0][pos];
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSequence(SEPtr<Labeling> evaluator,
                                               size_t begin,
                                               size_t end,
                                               size_t phase) const {
  return Base::evaluateSequence(evaluator, begin, end, phase);
}

/*----------------------------------------------------------------------------*/

void GeneralizedHiddenMarkovModel::initializeCache(CEPtr<Multiple> evaluator,
                                                   size_t phase) {
  Base::initializeCache(evaluator, phase);
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSymbol(CEPtr<Multiple> evaluator,
                                             size_t pos,
                                             size_t phase) const {
  return Base::evaluateSymbol(evaluator, pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSequence(CEPtr<Multiple> evaluator,
                                               size_t begin,
                                               size_t end,
                                               size_t phase) const {
  return Base::evaluateSequence(evaluator, begin, end, phase);
}

/*----------------------------------------------------------------------------*/

void GeneralizedHiddenMarkovModel::initializeCache(CEPtr<Labeling> evaluator,
                                                   size_t phase) {
  Base::initializeCache(evaluator, phase);
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSymbol(CEPtr<Labeling> evaluator,
                                             size_t pos,
                                             size_t phase) const {
  return Base::evaluateSymbol(evaluator, pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSequence(CEPtr<Labeling> evaluator,
                                               size_t begin,
                                               size_t end,
                                               size_t phase) const {
  return Base::evaluateSequence(evaluator, begin, end, phase);
}

/*===============================  GENERATOR  ================================*/

Multiple<Symbol>
GeneralizedHiddenMarkovModel::drawSymbol(
    SGPtr<Multiple> /* generator */,
    size_t /* position */,
    size_t /* phase */,
    const Multiple<Sequence>& /* sequence */) const {
  return {};
}

/*----------------------------------------------------------------------------*/

Multiple<Sequence>
GeneralizedHiddenMarkovModel::drawSequence(
    SGPtr<Multiple> generator,
    size_t size,
    size_t phase) const {
  return Base::drawSequence(generator, size, phase);
}

/*----------------------------------------------------------------------------*/

Labeling<Symbol>
GeneralizedHiddenMarkovModel::drawSymbol(
    SGPtr<Labeling> /* generator */,
    size_t /* position */,
    size_t /* phase */,
    const Labeling<Sequence>& /* sequence */) const {
  return {};
}

/*----------------------------------------------------------------------------*/

Labeling<Sequence>
GeneralizedHiddenMarkovModel::drawSequence(
    SGPtr<Labeling> generator,
    size_t size,
    size_t phase) const {
  return Base::drawSequence(generator, size, phase);
}

/*================================  LABELER  =================================*/

Estimation<Labeling<Sequence>>
GeneralizedHiddenMarkovModel::labeling(
    SLPtr /* labeler */, const Labeler::method& /* method */) const {
  return {};
}

/*----------------------------------------------------------------------------*/

void GeneralizedHiddenMarkovModel::initializeCache(CLPtr /* labeler */) {
}

/*----------------------------------------------------------------------------*/

Estimation<Labeling<Sequence>>
GeneralizedHiddenMarkovModel::labeling(
    CLPtr /* labeler */, const Labeler::method& /* method */) const {
  return {};
}

/*===============================  CALCULATOR  ===============================*/

Probability GeneralizedHiddenMarkovModel::calculate(
    SCPtr /* calculator */,
    const Calculator::direction& /* direction */) const {
  return {};
}

/*----------------------------------------------------------------------------*/

void GeneralizedHiddenMarkovModel::initializeCache(CCPtr /* calculator */) {
}

/*----------------------------------------------------------------------------*/

Probability GeneralizedHiddenMarkovModel::calculate(
    CCPtr /* calculator */,
    const Calculator::direction& /* direction */) const {
  return {};
}

/*===============================  SERIALIZER  ===============================*/

void GeneralizedHiddenMarkovModel::serialize(const SSPtr serializer) {
  Base::serialize(serializer);
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::GeneratorReturn<Symbol>
GeneralizedHiddenMarkovModel::drawSymbol(const RandomNumberGeneratorPtr& rng,
                                         size_t pos,
                                         const Sequence& context) const {
  assert(!context.empty() && context[0] == _begin_id);

  // TODO(renatocf): Implement drawing of duration
  Symbol label = _states[context[pos-1]]->transition()->draw(rng);

  Multiple<Symbol> alignment = {
    _states[label]->emission()->standardGenerator(rng)->drawSymbol(pos) };

  return { label, alignment };
}

/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::GeneratorReturn<Sequence>
GeneralizedHiddenMarkovModel::drawSequence(const RandomNumberGeneratorPtr& rng,
                                           size_t size) const {
  Multiple<Sequence> alignment(1);
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

/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::LabelerReturn<1>
GeneralizedHiddenMarkovModel::viterbi(const Multiple<Sequence>& sequences) const {
  Probability zero;

  auto gammas = make_multiarray(
      zero, _state_alphabet_size, sequences[0].size() + 1);

  auto phi = make_multiarray(
      _begin_id, _state_alphabet_size, sequences[0].size() + 1);

  auto psi = make_multiarray(
      _begin_id, _state_alphabet_size, sequences[0].size() + 1);

  // Initialization
  gammas[_begin_id][0] = 1;

  // Recursion
  for (size_t i = 0; i <= sequences[0].size(); i++) {
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
          Probability candidate_max
            = gammas[p][i - d]
            * _states[p]->transition()
                        ->probabilityOf(k)
            * _states[k]->duration()
                        ->probabilityOfLenght(d)
            * _states[k]->emission()
                        ->standardEvaluator(sequences)
                        ->evaluateSequence(begin, end, phase);

          if (candidate_max > gammas[k][i]) {
            gammas[k][i] = candidate_max;
            phi[k][i] = d;
            psi[k][i] = p;
          }
        }
      }
    }
  }

  // Termination
  auto max = gammas[_end_id][sequences[0].size()];
  auto[ label, alignment ] = traceBack(sequences, psi, phi);

  return { max, label, alignment, gammas };
}

/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::LabelerReturn<1>
GeneralizedHiddenMarkovModel::posteriorDecoding(
    const Multiple<Sequence>& sequences) const {
  const Probability zero;

  auto posteriors = make_multiarray(
      zero, _state_alphabet_size, sequences[0].size() + 1);

  auto phi = make_multiarray(
      _begin_id, _state_alphabet_size, sequences[0].size() + 1);

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
          * ((alphas[k][i] * betas[k][i]) / full);

        if (candidate_max > posteriors[k][i]) {
          posteriors[k][i] = candidate_max;
          phi[k][i] = 1;
          psi[k][i] = p;
        }
      }
    }
  }

  // Termination
  auto max = posteriors[_end_id][sequences[0].size()];
  auto[ label, alignment ] = traceBack(sequences, psi, phi);

  return { max, label, alignment, posteriors };
}

/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::CalculatorReturn<1>
GeneralizedHiddenMarkovModel::forward(const Multiple<Sequence>& sequences) const {
  const Probability zero;

  auto alphas = make_multiarray(
      zero, _state_alphabet_size, sequences[0].size() + 1);

  // Initialization
  alphas[_begin_id][0] = 1;

  // Recursion
  for (size_t i = 0; i <= sequences[0].size(); i++) {
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
          alphas[k][i]
            += alphas[p][i - d]
            * _states[p]->transition()
                        ->probabilityOf(k)
            * _states[k]->duration()
                        ->probabilityOfLenght(d)
            * _states[k]->emission()
                        ->standardEvaluator(sequences)
                        ->evaluateSequence(begin, end, phase);
        }
      }
    }
  }

  // Termination
  Probability full = alphas[_end_id][sequences[0].size()];

  return { full, alphas };
}

/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::CalculatorReturn<1>
GeneralizedHiddenMarkovModel::backward(const Multiple<Sequence>& sequences) const {
  const Probability zero;

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

        auto phase = _states[s]->beginPhase();

        auto max_length = std::min(sequences[0].size()-i, _max_backtracking);
        for (auto d : _states[s]->duration()->possibleLengths(max_length)) {
          auto begin = i, end = i+d;

          if (!segmentIsViable(sequences[0], begin, end, _states[s])) {
            continue;
          }

          betas[k][i]
            += _states[k]->transition()
                         ->probabilityOf(s)
            * _states[s]->duration()
                        ->probabilityOfLenght(d)
            * _states[s]->emission()
                        ->standardEvaluator(sequences)
                        ->evaluateSequence(begin, end, phase)
            * betas[s][i + d];
        }
      }
    }
  }

  // Termination
  Probability full = betas[_begin_id][0];

  return { full, betas };
}

/*----------------------------------------------------------------------------*/

bool GeneralizedHiddenMarkovModel::segmentIsViable(
    const Sequence& sequence,
    size_t begin,
    size_t end,
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
                   ->standardEvaluator({ sequence })
                   ->evaluateSequence(extended_begin,
                                      extended_end,
                                      extended_phase);

  return prob != zero;
}

/*----------------------------------------------------------------------------*/

typename GeneralizedHiddenMarkovModel::TraceBackReturn
GeneralizedHiddenMarkovModel::traceBack(
    const Multiple<Sequence>& sequences,
    const MultiArray<typename State::Id, 2>& psi,
    const MultiArray<size_t, 2>& phi) const {
  Sequence label;
  Multiple<Sequence> alignment(1);

  // Initialization
  std::vector<size_t> idxs { sequences[0].size() };

  auto best_id = psi[_end_id][idxs[0]];

  // Iteration
  label.push_back(_end_id);
  while (best_id != _begin_id) {
    auto duration = phi[best_id][idxs[0]];

    label.insert(label.end(), duration, best_id);

    if (_states[best_id]->hasGap(0)) {
      alignment[0].insert(alignment[0].end(), duration, _gap);
    } else {
      alignment[0].insert(
          alignment[0].end(),
          sequences[0].begin() + idxs[0] - duration,
          sequences[0].begin() + idxs[0]);
    }

    best_id = psi[best_id][idxs[0]];

    if (!_states[best_id]->hasGap(0)) { idxs[0] -= duration; }
  }
  label.push_back(_begin_id);

  // Termination
  std::reverse(label.begin(), label.end());
  std::reverse(alignment[0].begin(), alignment[0].end());

  return { label, alignment };
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
