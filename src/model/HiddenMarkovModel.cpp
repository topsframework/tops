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
#include "model/HiddenMarkovModel.hpp"

// Standard headers
#include <cmath>
#include <limits>
#include <vector>
#include <utility>
#include <algorithm>

// Internal headers
#include "model/Util.hpp"

#include "exception/NotYetImplemented.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

HiddenMarkovModel::HiddenMarkovModel(
    std::vector<StatePtr> states,
    DiscreteIIDModelPtr initial_probabilities,
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size)
    : Base(std::move(states), initial_probabilities,
           state_alphabet_size, observation_alphabet_size) {
}

/*----------------------------------------------------------------------------*/
/*                              STATIC METHODS                                */
/*----------------------------------------------------------------------------*/

HiddenMarkovModelPtr
HiddenMarkovModel::train(TrainerPtr<Standard, Self> trainer,
                         baum_welch_algorithm,
                         HiddenMarkovModelPtr initial_model,
                         unsigned int max_iterations,
                         double diff_threshold) {
  auto& observation_training_set = trainer->training_set();

  auto state_alphabet_size = initial_model->stateAlphabetSize();
  auto observation_alphabet_size = initial_model->observationAlphabetSize();

  auto model = HiddenMarkovModel::make(*initial_model);

  for (const auto& training_sequence : observation_training_set) {
    double last = 0;
    for (unsigned int iteration = 0; iteration < max_iterations; iteration++) {
      Matrix alpha, beta;
      Probability P = model->forward(training_sequence, alpha);
      model->backward(training_sequence, beta);

      std::vector<Probability> pi(state_alphabet_size);
      {
        Probability sum = 0;
        for (unsigned int i = 0; i < state_alphabet_size; i++)
          sum += alpha[i][0] * beta[i][0];

        for (unsigned int i = 0; i < state_alphabet_size; i++)
          pi[i] = (alpha[i][0] * beta[i][0]) / sum;
      }

      Matrix A(state_alphabet_size,
               std::vector<Probability>(observation_alphabet_size));
      for (size_t i = 0; i < state_alphabet_size; i++)
        for (size_t j = 0; j < state_alphabet_size; j++)
          for (size_t t = 0; t < training_sequence.size()-1; t++)
            A[i][j] += alpha[i][t]
              * model->state(i)->transition()->probabilityOf(j)
              * model->state(j)->emission()->probabilityOf(
                  training_sequence[t+1])
              * beta[j][t+1];

      Matrix E(state_alphabet_size,
               std::vector<Probability>(state_alphabet_size));
      for (size_t i = 0; i < state_alphabet_size; i++)
        for (size_t sigma = 0; sigma < observation_alphabet_size; sigma++)
          for (size_t t = 0; t < training_sequence.size(); t++)
            if (sigma == training_sequence[t])
              E[i][sigma] += alpha[i][t] * beta[i][t];

      std::vector<Probability> sumA(state_alphabet_size);
      std::vector<Probability> sumE(state_alphabet_size);
      for (unsigned int k = 0; k < state_alphabet_size; k++) {
        sumA[k] = std::accumulate(A[k].begin(), A[k].end(), Probability(0.0));
        sumE[k] = std::accumulate(E[k].begin(), E[k].end(), Probability(0.0));
      }

      std::vector<StatePtr> states(state_alphabet_size);
      for (size_t k = 0; k < state_alphabet_size; k++) {
        for (size_t l = 0; l < state_alphabet_size; l++)
          A[k][l] /= sumA[k];
        for (size_t b = 0; b < observation_alphabet_size; b++)
          E[k][b] /= sumE[k];

        states[k] = State::make(
          k, DiscreteIIDModel::make(E[k]), DiscreteIIDModel::make(A[k]));
      }

      model = HiddenMarkovModel::make(
        states, DiscreteIIDModel::make(pi),
        state_alphabet_size, observation_alphabet_size);

      auto diff = std::fabs(last - P.data());
      last = P.data();

      if (diff < diff_threshold) break;
    }
  }

  return model;
}

/*----------------------------------------------------------------------------*/

HiddenMarkovModelPtr
HiddenMarkovModel::train(TrainerPtr<Labeling, Self> trainer,
                         maximum_likehood_algorithm,
                         unsigned int state_alphabet_size,
                         unsigned int observation_alphabet_size,
                         double pseudocount) {
  auto& training_set = trainer->training_set();

  std::vector<double> initial_count(state_alphabet_size, pseudocount);

  using CountMatrix = std::vector<std::vector<double>>;
  CountMatrix emission_count(
      state_alphabet_size,
      std::vector<double>(observation_alphabet_size, pseudocount));
  CountMatrix transition_count(
      state_alphabet_size,
      std::vector<double>(state_alphabet_size, pseudocount));

  for (unsigned int i = 0; i < training_set.size(); i++) {
    initial_count[training_set[i].label()[0]] += 1;
    for (unsigned int j = 0; j < training_set[i].observation().size(); j++) {
      emission_count[training_set[i].label()[j]]
                    [training_set[i].observation()[j]] += 1;
      if (j < training_set[i].label().size() - 1) {
        transition_count[training_set[i].label()[j]]
                        [training_set[i].label()[j+1]] += 1;
      }
    }
  }

  std::vector<StatePtr> states(state_alphabet_size);
  for (unsigned int i = 0; i < state_alphabet_size; i++) {
    auto transition = DiscreteIIDModel::normalize(transition_count[i]);
    auto emission = DiscreteIIDModel::normalize(emission_count[i]);
    states[i] = State::make(i, DiscreteIIDModel::make(emission),
                               DiscreteIIDModel::make(transition));
  }

  auto initial_probabilities = DiscreteIIDModel::normalize(initial_count);
  return make(states, DiscreteIIDModel::make(initial_probabilities),
              state_alphabet_size, observation_alphabet_size);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*                           (Probabilistic Model)                            */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

Probability
HiddenMarkovModel::evaluateSymbol(SEPtr<Standard> /* evaluator */,
                                  unsigned int /* pos */,
                                  unsigned int /* phase */) const {
  throw_exception(NotYetImplemented);
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSequence(SEPtr<Standard> evaluator,
                                    unsigned int begin,
                                    unsigned int end,
                                    unsigned int /* phase */) const {
  Matrix alpha;
  forward(evaluator->sequence(), alpha);

  Probability sum_begin = 0;
  Probability sum_end = 0;

  for (unsigned int k = 0; k < _state_alphabet_size; k++) {
    sum_end += alpha[k][end-1];
    if (begin != 0)
      sum_begin += alpha[k][begin-1];
    else
      sum_begin = 1;
  }

  return sum_end / sum_begin;
}

/*----------------------------------------------------------------------------*/

void HiddenMarkovModel::initializeCache(CEPtr<Standard> evaluator,
                                        unsigned int /* phase */) {
  auto& cache = evaluator->cache();

  cache.prefix_sum_array.resize(evaluator->sequence().size() + 1);
  forward(evaluator->sequence(), cache.alpha);

  for (unsigned int i = 0; i < evaluator->sequence().size(); i++) {
    cache.prefix_sum_array[i] = 0;
    for (unsigned int k = 0; k < _state_alphabet_size; k++)
      cache.prefix_sum_array[i+1] += cache.alpha[k][i];
  }
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSymbol(CEPtr<Standard> /* evaluator */,
                                  unsigned int /* pos */,
                                  unsigned int /* phase */) const {
  throw_exception(NotYetImplemented);
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSequence(CEPtr<Standard> evaluator,
                                    unsigned int begin,
                                    unsigned int end,
                                    unsigned int /* phase */) const {
  return evaluator->cache().prefix_sum_array[end]
         / evaluator->cache().prefix_sum_array[begin];
}

/*===============================  GENERATOR  ================================*/

Standard<Symbol>
HiddenMarkovModel::drawSymbol(SGPtr<Standard> /* generator */,
                              unsigned int /* pos */,
                              unsigned int /* phase */,
                              const Sequence &/* context */) const {
  throw_exception(NotYetImplemented);
}

/*----------------------------------------------------------------------------*/

Standard<Sequence>
HiddenMarkovModel::drawSequence(SGPtr<Standard> /* generator */,
                                unsigned int /* size */,
                                unsigned int /* phase */) const {
  throw_exception(NotYetImplemented);
}

/*===============================  SERIALIZER  ===============================*/

void HiddenMarkovModel::serialize(SSPtr serializer) {
  Base::serialize(serializer);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*                             (Decodable Model)                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

Probability HiddenMarkovModel::evaluateSymbol(SEPtr<Labeling> evaluator,
                                              unsigned int pos,
                                              unsigned int /* phase */) const {
  Probability transition;
  const Sequence& observation = evaluator->sequence().observation();
  const Sequence& label = evaluator->sequence().label();

  if (pos == 0)
    transition = _initial_probabilities
      ->standardEvaluator(label)->evaluateSymbol(0);
  else
    transition = _states[label[pos-1]]->transition()
      ->standardEvaluator(label)->evaluateSymbol(pos);

  return transition
    * _states[label[pos]]->emission()
      ->standardEvaluator(observation)->evaluateSymbol(pos);
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSequence(SEPtr<Labeling> evaluator,
                                    unsigned int begin,
                                    unsigned int end,
                                    unsigned int phase) const {
  Probability prob = 1;
  for (unsigned int i = begin; i < end; i++)
    prob *= evaluateSymbol(evaluator, i, phase);
  return prob;
}

/*----------------------------------------------------------------------------*/

void HiddenMarkovModel::initializeCache(CEPtr<Labeling> evaluator,
                                        unsigned int phase) {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(evaluator->sequence().observation().size() + 1);

  prefix_sum_array[0] = 1;
  for (unsigned int i = 0; i < evaluator->sequence().observation().size(); i++)
    prefix_sum_array[i+1]
      = prefix_sum_array[i] * evaluateSymbol(evaluator, i, phase);
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSymbol(CEPtr<Labeling> evaluator,
                                  unsigned int pos,
                                  unsigned int phase) const {
  return evaluateSymbol(static_cast<SEPtr<Labeling>>(evaluator), pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability HiddenMarkovModel::evaluateSequence(CEPtr<Labeling> evaluator,
                                                unsigned int begin,
                                                unsigned int end,
                                                unsigned int /*phase*/) const {
  return evaluator->cache().prefix_sum_array[end]
         / evaluator->cache().prefix_sum_array[begin];
}

/*===============================  GENERATOR  ================================*/

Labeling<Symbol> HiddenMarkovModel::drawSymbol(SGPtr<Labeling> generator,
                                               unsigned int pos,
                                               unsigned int /* phase */,
                                               const Sequence& context) const {
  auto rng = generator->randomNumberGenerator();
  Symbol y = (pos == 0) ? _initial_probabilities->draw(rng)
                        : _states[context[pos-1]]->transition()->draw(rng);
  Symbol x = _states[y]->emission()->draw(rng);

  return Labeling<Symbol>(x, Sequence{y});
}

/*----------------------------------------------------------------------------*/

Labeling<Sequence> HiddenMarkovModel::drawSequence(SGPtr<Labeling> generator,
                                                   unsigned int size,
                                                   unsigned int phase) const {
  Sequence x, y;
  for (unsigned int i = 0; i < size; i++) {
    auto symbol_labeling = drawSymbol(generator, i, phase, y);
    x.push_back(symbol_labeling.observation());
    y.push_back(symbol_labeling.label().back());
  }
  return Labeling<Sequence>(x, y);
}

/*================================  LABELER  =================================*/

Estimation<Labeling<Sequence>>
HiddenMarkovModel::labeling(SLPtr labeler,
                            const Labeler::method& method) const {
  Matrix probabilities;
  switch (method) {
    case Labeler::method::bestPath:
      return viterbi(labeler->sequence(), probabilities);
    case Labeler::method::posteriorDecoding:
      return posteriorDecoding(labeler->sequence(), probabilities);
  }
  return Estimation<Labeling<Sequence>>();
}

/*----------------------------------------------------------------------------*/

void HiddenMarkovModel::initializeCache(CLPtr /* labeler */) {
  // Postpone initialization to methods
}

/*----------------------------------------------------------------------------*/

Estimation<Labeling<Sequence>>
HiddenMarkovModel::labeling(CLPtr labeler,
                            const Labeler::method& method) const {
  switch (method) {
    case Labeler::method::bestPath:
      return viterbi(labeler->sequence(), labeler->cache().gamma);
    case Labeler::method::posteriorDecoding:
      return posteriorDecoding(labeler->sequence(),
             labeler->cache().posterior_decoding);
  }
  return Estimation<Labeling<Sequence>>();
}

/*==============================  CALCULATOR  ================================*/

Probability HiddenMarkovModel::calculate(
    SCPtr calculator, const Calculator::direction& direction) const {
  Matrix probabilities;
  switch (direction) {
    case Calculator::direction::forward:
      return forward(calculator->sequence(), probabilities);
    case Calculator::direction::backward:
      return backward(calculator->sequence(), probabilities);
  }

  return 0;
}

/*----------------------------------------------------------------------------*/

void HiddenMarkovModel::initializeCache(CCPtr /* calculator */) {
  // Postpone initialization to methods
}

/*----------------------------------------------------------------------------*/

Probability HiddenMarkovModel::calculate(
    CCPtr calculator, const Calculator::direction& direction) const {
  Matrix probabilities;
  switch (direction) {
    case Calculator::direction::forward:
      return forward(calculator->sequence(), calculator->cache().alpha);
    case Calculator::direction::backward:
      return backward(calculator->sequence(), calculator->cache().beta);
  }

  return 0;
}

/*=================================  OTHERS  =================================*/

void HiddenMarkovModel::posteriorProbabilities(const Sequence& sequence,
                                               Matrix& probabilities) const {
  probabilities = std::vector<std::vector<Probability>>(
      _state_alphabet_size, std::vector<Probability>(sequence.size()));

  Matrix alpha;  // forward
  Matrix beta;   // backward

  Probability full = forward(sequence, alpha);
  backward(sequence, beta);

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    for (unsigned int i = 0; i < sequence.size(); i++)
      probabilities[k][i] = (alpha[k][i] * beta[k][i]) / full;
}

/*----------------------------------------------------------------------------*/
/*                             CONCRETE METHODS                               */
/*----------------------------------------------------------------------------*/

Estimation<Labeling<Sequence>>
HiddenMarkovModel::viterbi(const Sequence& xs,
                           Matrix& gamma) const {
  gamma = std::vector<std::vector<Probability>>(
      _state_alphabet_size, std::vector<Probability>(xs.size()));
  Matrix psi(_state_alphabet_size, std::vector<Probability>(xs.size()));

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    gamma[k][0] = _initial_probabilities->probabilityOf(k)
        * _states[k]->emission()->probabilityOf(xs[0]);

  for (unsigned int i = 0; i < xs.size() - 1; i++) {
    for (unsigned int k = 0; k < _state_alphabet_size; k++) {
      gamma[k][i+1] = gamma[0][i]
          * _states[0]->transition()->probabilityOf(k);
      psi[k][i+1] = 0;
      for (unsigned int p = 1; p < _state_alphabet_size; p++) {
        Probability v
          = gamma[p][i] * _states[p]->transition()->probabilityOf(k);
        if (gamma[k][i+1] < v) {
          gamma[k][i+1] = v;
          psi[k][i+1] = p;
        }
      }
      gamma[k][i+1] *= _states[k]->emission()->probabilityOf(xs[i+1]);
    }
  }

  Sequence ys(xs.size());
  ys[xs.size() - 1] = 0;
  Probability max = gamma[0][xs.size() - 1];
  for (unsigned int k = 1; k < _state_alphabet_size; k++) {
    if (max < gamma[k][xs.size() - 1]) {
      max = gamma[k][xs.size() - 1];
      ys[xs.size() - 1] = k;
    }
  }
  for (int i = xs.size() - 1; i >= 1; i--) {
    ys[i-1] = psi[ys[i]][i];
  }

  return Estimation<Labeling<Sequence>>(
      Labeling<Sequence>(xs, std::move(ys)), max);
}

/*----------------------------------------------------------------------------*/

Estimation<Labeling<Sequence>>
HiddenMarkovModel::posteriorDecoding(const Sequence& xs,
                                     Matrix& probabilities) const {
  posteriorProbabilities(xs, probabilities);
  Sequence path(xs.size());

  for (unsigned int i = 0; i < xs.size(); i++) {
    Probability max = probabilities[0][i];
    path[i] = 0;
    for (unsigned int k = 1; k < _state_alphabet_size; k++) {
      if (probabilities[k][i] > max) {
        max = probabilities[k][i];
        path[i] = k;
      }
    }
  }

  auto labeling = Labeling<Sequence>(xs, std::move(path));
  auto probability
    = const_cast<HiddenMarkovModel*>(this)
        ->labelingEvaluator(labeling)->evaluateSequence(0, xs.size());

  return Estimation<Labeling<Sequence>>(labeling, probability);
}

/*----------------------------------------------------------------------------*/

Probability HiddenMarkovModel::forward(const Sequence& seq,
                                       Matrix& alpha) const {
  alpha = Matrix(_state_alphabet_size, std::vector<Probability>(seq.size()));

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    alpha[k][0] = _initial_probabilities->probabilityOf(k)
      * _states[k]->emission()->probabilityOf(seq[0]);

  for (unsigned int t = 0; t < seq.size() - 1; t++) {
    for (unsigned int i = 0; i < _state_alphabet_size; i++) {
      for (unsigned int j = 0; j < _state_alphabet_size; j++) {
        alpha[i][t+1] +=
          alpha[j][t] * _states[j]->transition()->probabilityOf(i);
      }
      alpha[i][t+1] *= _states[i]->emission()->probabilityOf(seq[t+1]);
    }
  }

  Probability sum = 0;
  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    sum += alpha[k][seq.size()-1];

  return sum;
}

/*----------------------------------------------------------------------------*/

Probability HiddenMarkovModel::backward(const Sequence& seq,
                                        Matrix& beta) const {
  beta = Matrix(_state_alphabet_size, std::vector<Probability>(seq.size()));

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    beta[k][seq.size()-1] = 1.0;

  for (int t = seq.size()-2; t >= 0; t--) {
    for (unsigned int i = 0; i < _state_alphabet_size; i++) {
      for (unsigned int j = 0; j < _state_alphabet_size; j++) {
        beta[i][t] +=
          _states[i]->transition()->probabilityOf(j)
          * _states[j]->emission()->probabilityOf(seq[t+1])
          * beta[j][t+1];
      }
    }
  }

  Probability sum = 0;
  for (unsigned int k = 0; k < _state_alphabet_size; k++) {
    sum += beta[k][0]
      * _initial_probabilities->probabilityOf(k)
      * _states[k]->emission()->probabilityOf(seq[0]);
  }

  return sum;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
