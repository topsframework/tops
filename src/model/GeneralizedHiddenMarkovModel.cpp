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
#include "model/Segment.hpp"

// Interface header
#include "model/GeneralizedHiddenMarkovModel.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

GeneralizedHiddenMarkovModel::GeneralizedHiddenMarkovModel(
    std::vector<StatePtr> states,
    DiscreteIIDModelPtr initial_probabilities,
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size,
    unsigned int max_backtracking)
    : Base(std::move(states), initial_probabilities,
           state_alphabet_size, observation_alphabet_size),
      _max_backtracking(max_backtracking) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

void
GeneralizedHiddenMarkovModel::initializeCache(CEPtr<Standard> /* evaluator */,
                                              unsigned int /* phase */) {
  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSymbol(CEPtr<Standard> /* evaluator */,
                                             unsigned int /* pos */,
                                             unsigned int /* phase */) const {
  return -std::numeric_limits<Probability>::infinity();  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSequence(CEPtr<Standard> /* evaluator */,
                                               unsigned int /* begin */,
                                               unsigned int /* end */,
                                               unsigned int /* phase */) const {
  return -std::numeric_limits<Probability>::infinity();  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSymbol(SEPtr<Standard> /* evaluator */,
                                             unsigned int /* pos */,
                                             unsigned int /* phase */) const {
  return -std::numeric_limits<Probability>::infinity();  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSequence(SEPtr<Standard> /* evaluator */,
                                               unsigned int /* begin */,
                                               unsigned int /* end */,
                                               unsigned int /* phase */) const {
  return -std::numeric_limits<Probability>::infinity();  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

void
GeneralizedHiddenMarkovModel::initializeCache(CEPtr<Labeling> /* evaluator */,
                                              unsigned int /* phase */) {
  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSymbol(CEPtr<Labeling> evaluator,
                                             unsigned int pos,
                                             unsigned int phase) const {
  return evaluateSymbol(static_cast<SEPtr<Labeling>>(evaluator), pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSequence(CEPtr<Labeling> evaluator,
                                               unsigned int begin,
                                               unsigned int end,
                                               unsigned int phase) const {
  return evaluateSequence(
    static_cast<SEPtr<Labeling>>(evaluator), begin, end, phase);
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSymbol(SEPtr<Labeling> /* evaluator */,
                                             unsigned int /* pos */,
                                             unsigned int /* phase */) const {
  return -std::numeric_limits<Probability>::infinity();  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Probability
GeneralizedHiddenMarkovModel::evaluateSequence(SEPtr<Labeling> evaluator,
                                               unsigned int /* begin */,
                                               unsigned int /* end */,
                                               unsigned int /* phase */) const {
  Probability prob = 0;
  auto segments = Segment::readSequence(evaluator->sequence().label());
  for (unsigned int i = 0; i < segments.size(); i++) {
    if (i == 0) {
      prob += _initial_probabilities->probabilityOf(segments[i].symbol());
    } else {
      prob += _states[segments[i-1].symbol()]->transition()->probabilityOf(
        segments[i].symbol());
    }
    prob += _states[segments[i].symbol()]->duration()->probabilityOfLenght(
      segments[i].end() - segments[i].begin());
    prob += _states[segments[i].symbol()]->emission()->standardEvaluator(
      evaluator->sequence().observation())->evaluateSequence(
        segments[i].begin(), segments[i].end());
  }
  return prob;
}

/*===============================  GENERATOR  ================================*/

Standard<Symbol>
GeneralizedHiddenMarkovModel::drawSymbol(SGPtr<Standard> /* generator */,
                                         unsigned int /* pos */,
                                         unsigned int /* phase */,
                                         const Sequence &/* context */) const {
  return Standard<Symbol>(INVALID_SYMBOL);  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Labeling<Symbol>
GeneralizedHiddenMarkovModel::drawSymbol(SGPtr<Labeling> /* generator */,
                                         unsigned int /* pos */,
                                         unsigned int /* phase */,
                                         const Sequence &/* context */) const {
  return Labeling<Symbol>(INVALID_SYMBOL, INVALID_SYMBOL);  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Labeling<Sequence>
GeneralizedHiddenMarkovModel::drawSequence(SGPtr<Labeling> /* generator */,
                                           unsigned int size,
                                           unsigned int /* phase */) const {
  // TODO(igorbonadio)
  return Labeling<Sequence>(Sequence(INVALID_SYMBOL, size),
                            Sequence(INVALID_SYMBOL, size));
}

/*================================  LABELER  =================================*/

void
GeneralizedHiddenMarkovModel::initializeCache(CLPtr labeler) {
  labeler->cache().observation_evaluators
    = initializeObservationEvaluators(labeler->sequence(), true);
}

/*----------------------------------------------------------------------------*/

Estimation<Labeling<Sequence>>
GeneralizedHiddenMarkovModel::labeling(CLPtr labeler,
                                       const Labeler::method &method) const {
  switch (method) {
    case Labeler::method::bestPath:
      return viterbi(labeler->sequence(), labeler->cache().gamma,
                     labeler->cache().observation_evaluators);
    case Labeler::method::posteriorDecoding:
      return posteriorDecoding(labeler->sequence(),
                               labeler->cache().posterior_decoding);
  }
  return Estimation<Labeling<Sequence>>();
}

/*----------------------------------------------------------------------------*/

Estimation<Labeling<Sequence>>
GeneralizedHiddenMarkovModel::labeling(SLPtr labeler,
                                       const Labeler::method &method) const {
  Matrix probabilities;
  auto observation_evaluators
    = initializeObservationEvaluators(labeler->sequence(), false);

  switch (method) {
    case Labeler::method::bestPath:
      return viterbi(labeler->sequence(), probabilities,
                     observation_evaluators);
    case Labeler::method::posteriorDecoding:
      return posteriorDecoding(labeler->sequence(), probabilities);
  }
  return Estimation<Labeling<Sequence>>();
}

/*==============================  CALCULATOR  ================================*/

void GeneralizedHiddenMarkovModel::initializeCache(CCPtr calculator) {
  calculator->cache().observation_evaluators
    = initializeObservationEvaluators(calculator->sequence(), true);
}

/*----------------------------------------------------------------------------*/

Probability GeneralizedHiddenMarkovModel::calculate(
    SCPtr calculator, const Calculator::direction &direction) const {
  Matrix probabilities;
  auto observation_evaluators
    = initializeObservationEvaluators(calculator->sequence(), false);

  switch (direction) {
    case Calculator::direction::forward:
      forward(calculator->sequence(), probabilities, observation_evaluators);
    case Calculator::direction::backward:
      backward(calculator->sequence(), probabilities, observation_evaluators);
  }
  // TODO(igorbonadio)
  return -1;
}

/*----------------------------------------------------------------------------*/

Probability GeneralizedHiddenMarkovModel::calculate(
    CCPtr calculator, const Calculator::direction &direction) const {
  Matrix probabilities;

  switch (direction) {
    case Calculator::direction::forward:
      forward(calculator->sequence(),
              probabilities,
              calculator->cache().observation_evaluators);
    case Calculator::direction::backward:
      backward(calculator->sequence(),
               probabilities,
               calculator->cache().observation_evaluators);
  }
  // TODO(igorbonadio)
  return -1;
}

/*=================================  OTHERS  =================================*/


void GeneralizedHiddenMarkovModel::posteriorProbabilities(
    const Sequence &sequence,
    Matrix &probabilities) const {
  probabilities = std::vector<std::vector<Probability>>(
      _state_alphabet_size,
      std::vector<Probability>(sequence.size()));

  Matrix alpha;  // forward
  Matrix beta;   // backward

  auto observation_evaluators
    = initializeObservationEvaluators(sequence, false);

  Probability full = forward(sequence, alpha, observation_evaluators);
  backward(sequence, beta, observation_evaluators);

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    for (unsigned int i = 0; i < sequence.size(); i++)
      probabilities[k][i] = alpha[k][i] + beta[k][i] - full;
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

Estimation<Labeling<Sequence>> GeneralizedHiddenMarkovModel::viterbi(
      const Sequence &xs,
      Matrix &gamma,
      std::vector<EvaluatorPtr<Standard>> &observation_evaluators) const {
  gamma = std::vector<std::vector<Probability>>(
      _state_alphabet_size,
      std::vector<Probability>(xs.size()));
  Matrix psi(_state_alphabet_size, std::vector<Probability>(xs.size()));
  Matrix psilen(_state_alphabet_size, std::vector<Probability>(xs.size()));

  for (unsigned int i = 0; i < xs.size(); i++) {
    for (unsigned int k = 0; k < _state_alphabet_size; k++) {
      gamma[k][i] = -std::numeric_limits<Probability>::infinity();
      auto range = _states[k]->duration()->range();
      for (unsigned int d = range->begin();
           !range->end() && d <= (i + 1);
           d = range->next()) {
        unsigned int pmax = 0;
        Probability gmax;
        if (d > i) {
          gmax = _initial_probabilities->probabilityOf(k);
        } else {
          gmax = -std::numeric_limits<Probability>::infinity();
          for (auto p : _states[k]->predecessors()) {
            Probability g = gamma[p][i-d]
              + _states[p]->transition()->probabilityOf(k);
            if (gmax < g) {
              gmax = g;
              pmax = p;
            }
          }
        }
        gmax = gmax + _states[k]->duration()->probabilityOfLenght(d)
          + observation_evaluators[k]->evaluateSequence(i-d+1, i+1);
        if (gamma[k][i] < gmax) {
          gamma[k][i] = gmax;
          psi[k][i] = pmax;
          psilen[k][i] = d;
        }
      }
    }
  }

  unsigned int L = xs.size() - 1;
  Symbol state = 0;
  Probability max = gamma[0][L];
  for (unsigned int k = 1; k < _state_alphabet_size; k++) {
    if (max < gamma[k][L]) {
      max = gamma[k][L];
      state = k;
    }
  }

  Sequence path = Sequence(xs.size());

  unsigned int i = 0;
  while (i <= L) {
    unsigned int d = psilen[state][L-i];
    unsigned int p = psi[state][L-i];
    for (unsigned int j = 0; j < d; j++) {
      path[L-i] = state;
      i++;
    }
    state = p;
  }

  return Estimation<Labeling<Sequence>>(
      Labeling<Sequence>(xs, std::move(path)), max);
}

/*----------------------------------------------------------------------------*/

Estimation<Labeling<Sequence>>
GeneralizedHiddenMarkovModel::posteriorDecoding(const Sequence &xs,
                                                Matrix &probabilities) const {
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
  return Estimation<Labeling<Sequence>>(
      Labeling<Sequence>(xs, std::move(path)),
        -std::numeric_limits<Probability>::infinity());  // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Probability GeneralizedHiddenMarkovModel::forward(
    const Sequence &sequence,
    Matrix &alpha,
    std::vector<EvaluatorPtr<Standard>> &observation_evaluators) const {
  alpha = std::vector<std::vector<Probability>>(
    _state_alphabet_size, std::vector<Probability>(sequence.size()));

  for (unsigned int i = 0; i < sequence.size(); i++) {
    for (unsigned int k = 0; k < _state_alphabet_size; k++) {
      alpha[k][i] = -std::numeric_limits<Probability>::infinity();
      auto range = _states[k]->duration()->range();
      for (unsigned int d = range->begin();
           !range->end() && d <= (i + 1);
           d = range->next()) {
        if (d > i) {
          alpha[k][i] = log_sum(alpha[k][i],
            _initial_probabilities->probabilityOf(k)
              + _states[k]->duration()->probabilityOfLenght(d)
              + observation_evaluators[k]->evaluateSequence(i-d+1, i+1));
        } else {
          Probability sum = -std::numeric_limits<Probability>::infinity();
          for (auto p : _states[k]->predecessors()) {
            sum = log_sum(sum, alpha[p][i-d]
              + _states[p]->transition()->probabilityOf(k));
          }
          alpha[k][i] = log_sum(alpha[k][i],
            sum + _states[k]->duration()->probabilityOfLenght(d)
              + observation_evaluators[k]->evaluateSequence(i-d+1, i+1));
        }
      }
    }
  }

  Probability px = -std::numeric_limits<Probability>::infinity();
  for (unsigned int k = 0; k < _state_alphabet_size; k++) {
    px = log_sum(px, alpha[k][sequence.size()-1]);
  }

  return px;
}

/*----------------------------------------------------------------------------*/

Probability GeneralizedHiddenMarkovModel::backward(
    const Sequence &sequence,
    Matrix &beta,
    std::vector<EvaluatorPtr<Standard>> &observation_evaluators) const {
  beta = std::vector<std::vector<Probability>>(
    _state_alphabet_size, std::vector<Probability>(sequence.size()));

  for (unsigned int k = 0; k < _state_alphabet_size; k++) {
    beta[k][sequence.size()-1] = 0.0;
  }

  for (int i = sequence.size()-2; i >= 0; i--) {
    for (unsigned int k = 0; k < _state_alphabet_size; k++) {
      beta[k][i] = -std::numeric_limits<Probability>::infinity();
      for (auto p : _states[k]->successors()) {
        Probability sum = -std::numeric_limits<Probability>::infinity();
        auto range = _states[p]->duration()->range();
        for (unsigned int d = range->begin();
             !range->end() && d < (sequence.size() - i);
             d = range->next()) {
          sum = log_sum(sum,
            _states[p]->duration()->probabilityOfLenght(d)
              + observation_evaluators[p]->evaluateSequence(i+1, i+d+1)
              + beta[p][i+d]);
        }
        beta[k][i] = log_sum(beta[k][i],
                             _states[k]->transition()->probabilityOf(p) + sum);
      }
    }
  }

  Probability px = -std::numeric_limits<Probability>::infinity();
  for (unsigned int k = 0; k < _state_alphabet_size; k++) {
    Probability sum = -std::numeric_limits<Probability>::infinity();
    auto range = _states[k]->duration()->range();
    for (unsigned int d = range->begin();
         !range->end() && d <= (sequence.size());
         d = range->next()) {
      sum = log_sum(sum,
        _states[k]->duration()->probabilityOfLenght(d)
          + observation_evaluators[k]->evaluateSequence(0, d)
          + beta[k][d-1]);
    }
    px = log_sum(px, _initial_probabilities->probabilityOf(k) + sum);
  }

  return px;
}

/*----------------------------------------------------------------------------*/

std::vector<EvaluatorPtr<Standard>>
GeneralizedHiddenMarkovModel::initializeObservationEvaluators(
    const Sequence &xs, bool cached) const {
  std::vector<EvaluatorPtr<Standard>> observation_evaluators;
  for (auto state : _states) {
    observation_evaluators.push_back(
      state->emission()->standardEvaluator(xs, cached));
  }
  return observation_evaluators;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
