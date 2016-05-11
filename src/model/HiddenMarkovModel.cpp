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

// Standard headers
#include <cmath>
#include <limits>
#include <vector>
#include <algorithm>

// ToPS headers
#include "model/Util.hpp"

// Interface header
#include "model/HiddenMarkovModel.hpp"

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
                         unsigned int maxiterations,
                         double diff_threshold) {
  auto& observation_training_set = trainer->training_set();

  unsigned int state_alphabet_size
      = initial_model->stateAlphabetSize();
  unsigned int observation_alphabet_size
      = initial_model->observationAlphabetSize();

  double diff  = 10.0;

  auto model = initial_model;
  for (unsigned int s = 0; s < observation_training_set.size(); s++) {
    double last = 10000;
    for (unsigned int iterations = 0; iterations < maxiterations;
         iterations++) {
      std::vector<double> pi(state_alphabet_size);
      Matrix A(state_alphabet_size,
               std::vector<double>(observation_alphabet_size));
      Matrix E(state_alphabet_size,
               std::vector<double>(state_alphabet_size));

      Matrix alpha;
      Matrix beta;

      Probability P = model->forward(observation_training_set[s], alpha);
      model->backward(observation_training_set[s], beta);

      {
        Probability sum = alpha[0][0] + beta[0][0];
        for (unsigned int i = 1; i < state_alphabet_size; i++)
          sum = log_sum(sum, alpha[i][0] + beta[i][0]);

        for (unsigned int i = 0; i < state_alphabet_size; i++)
          pi[i] = alpha[i][0] + beta[i][0] - sum;
      }

      for (unsigned int i = 0; i < state_alphabet_size; i++) {
        for (unsigned int j = 0; j < state_alphabet_size; j++) {
          unsigned int t = 0;
          Probability sum = -std::numeric_limits<Probability>::infinity();
          if (t < observation_training_set[s].size()-1) {
            sum = alpha[i][t] + model->state(i)->transition()->probabilityOf(j)
                + model->state(j)->emission()->probabilityOf(
                    observation_training_set[s][t+1])
                + beta[j][t+1];
            for (t = 1; t < observation_training_set[s].size()-1; t++)
              sum = log_sum(sum,
                  alpha[i][t] + model->state(i)->transition()->probabilityOf(j)
                  + model->state(j)->emission()->probabilityOf(
                      observation_training_set[s][t+1])
                  + beta[j][t+1]);
          }
          A[i][j] = sum;
        }

        for (unsigned int sigma = 0; sigma < observation_alphabet_size;
            sigma++) {
          Probability sum = -std::numeric_limits<Probability>::infinity();
          bool first = true;
          for (unsigned int t = 0; t < observation_training_set[s].size();
              t++) {
            if ((sigma == observation_training_set[s][t]) && first) {
              sum = alpha[i][t] + beta[i][t];
              first = false;
            } else if (sigma == observation_training_set[s][t]) {
              sum = log_sum(sum, alpha[i][t] + beta[i][t]);
            }
          }
          E[i][sigma] = sum;
        }
      }

      std::vector<double> sumA(state_alphabet_size);
      std::vector<double> sumE(observation_alphabet_size);
      for (unsigned int k = 0; k < state_alphabet_size; k++) {
        unsigned int l = 0;
        if (l < state_alphabet_size) {
          sumA[k] = A[k][l];
          for (l = 1; l < state_alphabet_size; l++)
            sumA[k] = log_sum(sumA[k], A[k][l]);
        }
        unsigned int b = 0;
        if (b < observation_alphabet_size) {
          sumE[k] = E[k][b];
          for ( b = 1; b < observation_alphabet_size; b++)
            sumE[k] = log_sum(sumE[k], E[k][b]);
        }
      }

      std::vector<StatePtr> states(state_alphabet_size);
      for (unsigned int k = 0; k < state_alphabet_size; k++) {
        for (unsigned int l = 0; l < state_alphabet_size; l++) {
          A[k][l] = A[k][l] - sumA[k];
        }
        for (unsigned int b = 0; b <observation_alphabet_size; b++) {
          E[k][b] = E[k][b] - sumE[k];
        }
        states[k] = State::make(
          k,
          DiscreteIIDModel::make(E[k]),
          DiscreteIIDModel::make(A[k]));
      }

      model = HiddenMarkovModel::make(
        states,
        DiscreteIIDModel::make(pi),
        state_alphabet_size,
        observation_alphabet_size);

      diff = fabs(last - P);
      // std::cerr << "iteration: " << iterations << std::endl;
      // fprintf(stderr, "LL: %lf\n" , P );
      // std::cerr << "Diff: " << diff << std::endl;
      last = P;

      if (diff < diff_threshold)
        break;
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
                         double pseudocont) {
  auto& training_set = trainer->training_set();

  std::vector<double> initial_probabilities(state_alphabet_size, pseudocont);
  Matrix emissions(state_alphabet_size,
                   std::vector<double>(observation_alphabet_size, pseudocont));
  Matrix transitions(state_alphabet_size,
                     std::vector<double>(state_alphabet_size, pseudocont));

  for (unsigned int i = 0; i < training_set.size(); i++) {
    initial_probabilities[training_set[i].label()[0]] += 1.0;
    for (unsigned int j = 0; j < training_set[i].observation().size(); j++) {
      emissions[training_set[i].label()[j]][training_set[i].observation()[j]]
        += 1.0;
      if (j < training_set[i].label().size() - 1) {
        transitions[training_set[i].label()[j]][training_set[i].label()[j+1]]
          += 1.0;
      }
    }
  }

  initial_probabilities = DiscreteIIDModel::normalize(initial_probabilities);
  std::vector<StatePtr> states(state_alphabet_size);
  for (unsigned int i = 0; i < state_alphabet_size; i++) {
    transitions[i] = DiscreteIIDModel::normalize(transitions[i]);
    emissions[i] = DiscreteIIDModel::normalize(emissions[i]);
    states[i] = State::make(
      i,
      DiscreteIIDModel::make(emissions[i]),
      DiscreteIIDModel::make(transitions[i]));
  }

  return make(states,
              DiscreteIIDModel::make(initial_probabilities),
              state_alphabet_size,
              observation_alphabet_size);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

void HiddenMarkovModel::initializeCache(CEPtr<Standard> evaluator,
                                        unsigned int /* phase */) {
  initializeStandardPrefixSumArray(evaluator->sequence(), evaluator->cache());
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSymbol(CEPtr<Standard> /* evaluator */,
                                  unsigned int /* pos */,
                                  unsigned int /* phase */) const {
  return -std::numeric_limits<Probability>::infinity();  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSequence(CEPtr<Standard> evaluator,
                                    unsigned int begin,
                                    unsigned int end,
                                    unsigned int /* phase */) const {
  return evaluator->cache().prefix_sum_array[end]
         - evaluator->cache().prefix_sum_array[begin];
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSymbol(SEPtr<Standard> /* evaluator */,
                                  unsigned int /* pos */,
                                  unsigned int /* phase */) const {
  return -std::numeric_limits<Probability>::infinity();  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSequence(SEPtr<Standard> evaluator,
                                    unsigned int begin,
                                    unsigned int end,
                                    unsigned int /* phase */) const {
  Matrix alpha;
  forward(evaluator->sequence(), alpha);
  Probability sum_end = -std::numeric_limits<Probability>::infinity();
  Probability sum_begin = -std::numeric_limits<Probability>::infinity();
  for (unsigned int k = 0; k < _state_alphabet_size; k++) {
    sum_end = log_sum(sum_end, alpha[k][end-1]);
    if (begin != 0)
      sum_begin = log_sum(sum_begin, alpha[k][begin-1]);
    else
      sum_begin = 0;
  }
  return sum_end - sum_begin;
}

/*----------------------------------------------------------------------------*/

void HiddenMarkovModel::initializeCache(CEPtr<Labeling> evaluator,
                                        unsigned int phase) {
  initializeLabelingPrefixSumArray(evaluator, phase);
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSymbol(CEPtr<Labeling> evaluator,
                                  unsigned int pos,
                                  unsigned int phase) const {
  return evaluateSymbol(static_cast<SEPtr<Labeling>>(evaluator), pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSequence(CEPtr<Labeling> evaluator,
                                    unsigned int begin,
                                    unsigned int end,
                                    unsigned int /*phase*/) const {
  return evaluator->cache().prefix_sum_array[end]
         - evaluator->cache().prefix_sum_array[begin];
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSymbol(SEPtr<Labeling> evaluator,
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
    + _states[label[pos]]->emission()
      ->standardEvaluator(observation)->evaluateSymbol(pos);
}

/*----------------------------------------------------------------------------*/

Probability
HiddenMarkovModel::evaluateSequence(SEPtr<Labeling> evaluator,
                                    unsigned int begin,
                                    unsigned int end,
                                    unsigned int phase) const {
  Probability prob = 0;
  for (unsigned int i = begin; i < end; i++)
    prob += evaluateSymbol(evaluator, i, phase);
  return prob;
}

/*===============================  GENERATOR  ================================*/

Standard<Symbol>
HiddenMarkovModel::drawSymbol(SGPtr<Standard> /* generator */,
                              unsigned int /* pos */,
                              unsigned int /* phase */,
                              const Sequence &/* context */) const {
  // TODO(igorbonadio)
  return INVALID_SYMBOL;
}

/*----------------------------------------------------------------------------*/

Labeling<Symbol> HiddenMarkovModel::drawSymbol(SGPtr<Labeling> generator,
                                               unsigned int pos,
                                               unsigned int /* phase */,
                                               const Sequence& context) const {
  auto rng = generator->randomNumberGenerator();
  Symbol y = (pos == 0) ? _initial_probabilities->draw(rng)
                        : _states[context[pos-1]]->transition()->draw(rng);
  Symbol x = _states[y]->emission()->draw(rng);

  return Labeling<Symbol>(x, y);
}

/*----------------------------------------------------------------------------*/

Labeling<Sequence> HiddenMarkovModel::drawSequence(SGPtr<Labeling> generator,
                                                   unsigned int size,
                                                   unsigned int phase) const {
  Sequence x, y;
  for (unsigned int i = 0; i < size; i++) {
    auto symbol_labeling = drawSymbol(generator, i, phase, y);
    x.push_back(symbol_labeling.observation());
    y.push_back(symbol_labeling.label());
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

/*----------------------------------------------------------------------------*/

void HiddenMarkovModel::initializeCache(CLPtr /*labeler*/) {
  // TODO(igorbonadio)
}

/*==============================  CALCULATOR  ================================*/

void HiddenMarkovModel::initializeCache(CCPtr /*calculator*/) {
  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Probability HiddenMarkovModel::calculate(
    SCPtr calculator, const Calculator::direction& direction) const {
  Matrix probabilities;
  switch (direction) {
    case Calculator::direction::forward:
      return forward(calculator->sequence(), probabilities);
    case Calculator::direction::backward:
      return backward(calculator->sequence(), probabilities);
  }
  // TODO(igorbonadio)
  return -1;
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
  // TODO(igorbonadio)
  return -1;
}

/*=================================  OTHERS  =================================*/

void HiddenMarkovModel::posteriorProbabilities(const Sequence& sequence,
                                               Matrix& probabilities) const {
  probabilities = std::vector<std::vector<Probability>>(
      _state_alphabet_size,
      std::vector<Probability>(sequence.size()));

  Matrix alpha;  // forward
  Matrix beta;   // backward

  Probability full = forward(sequence, alpha);
  backward(sequence, beta);

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    for (unsigned int i = 0; i < sequence.size(); i++)
      probabilities[k][i] = alpha[k][i] + beta[k][i] - full;
}

/*----------------------------------------------------------------------------*/
/*                             CONCRETE METHODS                               */
/*----------------------------------------------------------------------------*/

void HiddenMarkovModel::initializeStandardPrefixSumArray(
    const Sequence& sequence, Cache& cache) {
  cache.prefix_sum_array.resize(sequence.size()+1);
  forward(sequence, cache.alpha);
  cache.prefix_sum_array[0] = 0;
  for (unsigned int i = 0; i < sequence.size(); i++) {
    cache.prefix_sum_array[i] = -std::numeric_limits<double>::infinity();
    for (unsigned int k = 0; k < _state_alphabet_size; k++) {
      cache.prefix_sum_array[i+1]
        = log_sum(cache.prefix_sum_array[i+1], cache.alpha[k][i]);
    }
  }
}

/*----------------------------------------------------------------------------*/

void HiddenMarkovModel::initializeLabelingPrefixSumArray(
    CEPtr<Labeling> evaluator, unsigned int phase) {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(evaluator->sequence().observation().size() + 1);

  prefix_sum_array[0] = 0;
  for (unsigned int i = 0; i < evaluator->sequence().observation().size(); i++)
    prefix_sum_array[i+1]
      = prefix_sum_array[i] + evaluateSymbol(evaluator, i, phase);
}

/*----------------------------------------------------------------------------*/

Estimation<Labeling<Sequence>>
HiddenMarkovModel::viterbi(const Sequence& xs,
                           Matrix& gamma) const {
  gamma = std::vector<std::vector<Probability>>(
      _state_alphabet_size,
      std::vector<Probability>(xs.size()));
  Matrix psi(_state_alphabet_size, std::vector<Probability>(xs.size()));

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    gamma[k][0] = _initial_probabilities->probabilityOf(k)
        + _states[k]->emission()->probabilityOf(xs[0]);

  for (unsigned int i = 0; i < xs.size() - 1; i++) {
    for (unsigned int k = 0; k < _state_alphabet_size; k++) {
      gamma[k][i+1] =  gamma[0][i]
          + _states[0]->transition()->probabilityOf(k);
      psi[k][i+1] = 0;
      for (unsigned int p = 1; p < _state_alphabet_size; p++) {
        Probability v
          = gamma[p][i] + _states[p]->transition()->probabilityOf(k);
        if (gamma[k][i+1] < v) {
          gamma[k][i+1] = v;
          psi[k][i+1] = p;
        }
      }
      gamma[k][i+1] += _states[k]->emission()->probabilityOf(xs[i+1]);
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
    = const_cast<HiddenMarkovModel *>(this)
        ->labelingEvaluator(labeling)->evaluateSequence(0, xs.size());

  return Estimation<Labeling<Sequence>>(labeling, probability);
}

/*----------------------------------------------------------------------------*/

Probability HiddenMarkovModel::forward(const Sequence& sequence,
                                  Matrix& alpha) const {
  alpha = std::vector<std::vector<Probability>>(
      _state_alphabet_size,
      std::vector<Probability>(sequence.size()));

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    alpha[k][0] = _initial_probabilities->probabilityOf(k)
        + _states[k]->emission()->probabilityOf(sequence[0]);

  for (unsigned int t = 0; t < sequence.size() - 1; t++) {
    for (unsigned int i = 0; i < _state_alphabet_size; i++) {
      alpha[i][t+1] = alpha[0][t] + _states[0]->transition()->probabilityOf(i);
      for (unsigned int j = 1; j < _state_alphabet_size; j++) {
        alpha[i][t+1] = log_sum(
            alpha[i][t+1],
            alpha[j][t] + _states[j]->transition()->probabilityOf(i));
      }
      alpha[i][t+1] += _states[i]->emission()->probabilityOf(sequence[t+1]);
    }
  }

  Probability sum =  alpha[0][sequence.size()-1];
  for (unsigned int k = 1; k < _state_alphabet_size; k++) {
    sum = log_sum(sum, alpha[k][sequence.size()-1]);
  }
  return sum;
}

/*----------------------------------------------------------------------------*/

Probability HiddenMarkovModel::backward(const Sequence& sequence,
                                   Matrix& beta) const {
  beta = std::vector<std::vector<Probability>>(
      _state_alphabet_size,
      std::vector<Probability>(sequence.size()));

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    beta[k][sequence.size()-1] = 0.0;

  for (int t = sequence.size()-2; t >= 0; t--) {
    for (unsigned int i = 0; i < _state_alphabet_size; i++) {
      beta[i][t] =  _states[i]->transition()->probabilityOf(0)
          + _states[0]->emission()->probabilityOf(sequence[t+1])
          + beta[0][t+1];
      for (unsigned int j = 1; j < _state_alphabet_size; j++) {
        beta[i][t] = log_sum(
            beta[i][t],
            _states[i]->transition()->probabilityOf(j)
                + _states[j]->emission()->probabilityOf(sequence[t+1])
                + beta[j][t+1]);
      }
    }
  }

  Probability sum = beta[0][0] + _initial_probabilities->probabilityOf(0)
      + _states[0]->emission()->probabilityOf(sequence[0]);
  for (unsigned int k = 1; k < _state_alphabet_size; k++) {
    sum = log_sum(sum, beta[k][0] + _initial_probabilities->probabilityOf(k)
        + _states[k]->emission()->probabilityOf(sequence[0]));
  }

  return sum;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
