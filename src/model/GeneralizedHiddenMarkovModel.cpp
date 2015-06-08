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
#include <vector>
#include <algorithm>

// ToPS headers
#include "model/GeneralizedHiddenMarkovModel.hpp"
#include "model/Util.hpp"
#include "model/Segment.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

GeneralizedHiddenMarkovModel::GeneralizedHiddenMarkovModel(
    std::vector<GeneralizedHiddenMarkovModelStatePtr> states,
    DiscreteIIDModelPtr initial_probabilities,
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size,
    unsigned int max_backtracking)
    : _states(states),
      _initial_probabilities(initial_probabilities),
      _state_alphabet_size(state_alphabet_size),
      _observation_alphabet_size(observation_alphabet_size),
      _max_backtracking(max_backtracking) {
}

/*----------------------------------------------------------------------------*/
/*                              STATIC METHODS                                */
/*----------------------------------------------------------------------------*/

GeneralizedHiddenMarkovModelPtr GeneralizedHiddenMarkovModel::make(
    std::vector<GeneralizedHiddenMarkovModelStatePtr> states,
    DiscreteIIDModelPtr initial_probability,
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size,
    unsigned int max_backtracking) {
  return GeneralizedHiddenMarkovModelPtr(new GeneralizedHiddenMarkovModel(
    states,
    initial_probability,
    state_alphabet_size,
    observation_alphabet_size,
    max_backtracking));
}

/*----------------------------------------------------------------------------*/
/*                             VIRTUAL METHODS                                */
/*----------------------------------------------------------------------------*/

double GeneralizedHiddenMarkovModel::forward(const Sequence &xs,
                                             Matrix &alpha) const {
  alpha = std::vector<std::vector<double>>(
    _state_alphabet_size, std::vector<double>(xs.size()));

  for (unsigned int i = 0; i < xs.size(); i++) {
    for (unsigned int k = 0; k < _state_alphabet_size; k++) {
      alpha[k][i] = -HUGE;
      auto durations = _states[k]->durations();
      for (unsigned int d = durations->begin();
           !durations->end() && d <= (i + 1);
           d = durations->next()) {
        if (d > i) {
          alpha[k][i] = log_sum(alpha[k][i],
            _initial_probabilities->probabilityOf(k)
              + _states[k]->durationProbability(d)
              + _states[k]->observation()->evaluator(xs)->probabilityOf(i-d+1,
                                                                        i+1));
        } else {
          double sum = -HUGE;
          for (auto p : _states[k]->predecessors()) {
            sum = log_sum(sum, alpha[p][i-d]
              + _states[p]->transition()->probabilityOf(k));
          }
          alpha[k][i] = log_sum(alpha[k][i],
            sum + _states[k]->durationProbability(d)
              + _states[k]->observation()->evaluator(xs)->probabilityOf(i-d+1,
                                                                        i+1));
        }
      }
    }
  }

  double px = -HUGE;
  for (unsigned int k = 0; k < _state_alphabet_size; k++) {
    px = log_sum(px, alpha[k][xs.size()-1]);
  }

  return px;
}

double GeneralizedHiddenMarkovModel::backward(const Sequence &xs,
                                              Matrix &beta) const {
  beta = std::vector<std::vector<double>>(
    _state_alphabet_size, std::vector<double>(xs.size()));

  for (unsigned int k = 0; k < _state_alphabet_size; k++) {
    beta[k][xs.size()-1] = 0.0;
  }

  for (int i = xs.size()-2; i >= 0; i--) {
    for (unsigned int k = 0; k < _state_alphabet_size; k++) {
      beta[k][i] = -HUGE;
      for (auto p : _states[k]->successors()) {
        double sum = -HUGE;
        auto durations = _states[p]->durations();
        for (unsigned int d = durations->begin();
             !durations->end() && d < (xs.size() - i);
             d = durations->next()) {
          sum = log_sum(sum,
            _states[p]->durationProbability(d)
              + _states[p]->observation()->evaluator(xs)->probabilityOf(i+1,
                                                                        i+d+1)
              + beta[p][i+d]);
        }
        beta[k][i] = log_sum(beta[k][i],
                             _states[k]->transition()->probabilityOf(p) + sum);
      }
    }
  }

  double px = -HUGE;
  for (unsigned int k = 0; k < _state_alphabet_size; k++) {
    double sum = -HUGE;
    auto durations = _states[k]->durations();
    for (unsigned int d = durations->begin();
         !durations->end() && d <= (xs.size());
         d = durations->next()) {
      sum = log_sum(sum,
        _states[k]->durationProbability(d)
          + _states[k]->observation()->evaluator(xs)->probabilityOf(0, d)
          + beta[k][d-1]);
    }
    px = log_sum(px, _initial_probabilities->probabilityOf(k) + sum);
  }

  return px;
}

double GeneralizedHiddenMarkovModel::evaluate(
    const Sequence &xs,
    const Sequence &ys) const {
  double prob = 0;
  auto segments = Segment::readSequence(ys);
  for (unsigned int i = 0; i < segments.size(); i++) {
    if (i == 0) {
      prob += _initial_probabilities->probabilityOf(segments[i].symbol());
    } else {
      prob += _states[segments[i-1].symbol()]->transition()->probabilityOf(
        segments[i].symbol());
    }
    prob += _states[segments[i].symbol()]->durationProbability(
      segments[i].end() - segments[i].begin());
    prob += _states[segments[i].symbol()]->observation()->evaluator(
      xs)->probabilityOf(segments[i].begin(), segments[i].end());
  }
  return prob;
}

double GeneralizedHiddenMarkovModel::evaluate(const Sequence &s,
                        unsigned int pos,
                        unsigned int phase) const {
  return -HUGE; // TODO(igorbonadio)
}

double GeneralizedHiddenMarkovModel::evaluate(
    const Sequence &xs,
    const Sequence &ys,
    unsigned int i) const {
  return -HUGE; // TODO(igorbonadio)
}

void GeneralizedHiddenMarkovModel::chooseSequences(Sequence &xs,
                             Sequence &ys,
                             unsigned int size) const {
  // TODO(igorbonadio)
}

void GeneralizedHiddenMarkovModel::chooseSequencesPosition(Sequence &xs,
                                     Sequence &ys,
                                     unsigned int i) const {
  // TODO(igorbonadio)
}

Symbol GeneralizedHiddenMarkovModel::choosePosition(const Sequence &xs,
                              unsigned int i,
                              unsigned int phase) const {
  return INVALID_SYMBOL; // TODO(igorbonadio)
}

Labeling GeneralizedHiddenMarkovModel::viterbi(const Sequence &xs,
                           Matrix &gamma) const {
  gamma = std::vector<std::vector<double>>(
      _state_alphabet_size,
      std::vector<double>(xs.size()));
  Matrix psi(_state_alphabet_size, std::vector<double>(xs.size()));
  Matrix psilen(_state_alphabet_size, std::vector<double>(xs.size()));

  for (unsigned int i = 0; i < xs.size(); i++) {
    for (unsigned int k = 0; k < _state_alphabet_size; k++) {
      gamma[k][i] = -HUGE;
      auto durations = _states[k]->durations();
      for (unsigned int d = durations->begin();
           !durations->end() && d <= (i + 1);
           d = durations->next()) {
        unsigned int pmax = 0;
        double gmax;
        if (d > i) {
          gmax = _initial_probabilities->probabilityOf(k);
        } else {
          gmax = -HUGE;
          for (auto p : _states[k]->predecessors()) {
            double g = gamma[p][i-d]
              + _states[p]->transition()->probabilityOf(k);
            if (gmax < g) {
              gmax = g;
              pmax = p;
            }
          }
        }
        gmax = gmax + _states[k]->durationProbability(d)
          + _states[k]->observation()->evaluator(xs)->probabilityOf(i-d+1,
                                                                    i+1);
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
  double max = gamma[0][L];
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

  return Labeling(max, std::move(path));
}

Labeling GeneralizedHiddenMarkovModel::posteriorDecoding(
    const Sequence &xs,
    Matrix &probabilities) const {
  posteriorProbabilities(xs, probabilities);

  Sequence path(xs.size());

  for (unsigned int i = 0; i < xs.size(); i++) {
    double max = probabilities[0][i];
    path[i] = 0;
    for (unsigned int k = 1; k < _state_alphabet_size; k++) {
      if (probabilities[k][i] > max) {
        max = probabilities[k][i];
        path[i] = k;
      }
    }
  }
  return Labeling(-HUGE, std::move(path)); // TODO(igorbonadio)
}

void GeneralizedHiddenMarkovModel::posteriorProbabilities(
    const Sequence &xs,
    Matrix &probabilities) const {
  probabilities = std::vector<std::vector<double>>(
      _state_alphabet_size,
      std::vector<double>(xs.size()));

  Matrix alpha;  // forward
  Matrix beta;   // backward

  double full = forward(xs, alpha);
  backward(xs, beta);

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    for (unsigned int i = 0; i < xs.size(); i++)
      probabilities[k][i] = alpha[k][i] + beta[k][i] - full;
}

Labeling GeneralizedHiddenMarkovModel::labeling(
    const Sequence &xs,
    Matrix &probabilities,
    Labeling::Method method) const {
  switch (method) {
    case Labeling::Method::bestPath:
      return viterbi(xs, probabilities);
    case Labeling::Method::posteriorDecoding:
      return posteriorDecoding(xs, probabilities);
  }
  return Labeling();
}

EvaluatorPtr GeneralizedHiddenMarkovModel::evaluator(
    const Sequence &s,
    bool cached) {
  return decodableEvaluator(s, cached);
}

DecodableEvaluatorPtr GeneralizedHiddenMarkovModel::decodableEvaluator(
    const Sequence &s,
    bool cached) {
  if (cached)
    return DecodableEvaluator::make(
      CachedEvaluatorImpl<GeneralizedHiddenMarkovModel>::make(
        std::static_pointer_cast<GeneralizedHiddenMarkovModel>(
          shared_from_this()),
        s, Cache()));
  return DecodableEvaluator::make(
    SimpleEvaluatorImpl<GeneralizedHiddenMarkovModel>::make(
      std::static_pointer_cast<GeneralizedHiddenMarkovModel>(
        shared_from_this()), s));
}

/*----------------------------------------------------------------------------*/
/*                             CONCRETE METHODS                               */
/*----------------------------------------------------------------------------*/

/*==============================  EVALUATOR  =================================*/

void GeneralizedHiddenMarkovModel::initializeCachedEvaluator(
    CEPtr evaluator, unsigned int phase) {
  // TODO(igorbonadio)
}

Labeling GeneralizedHiddenMarkovModel::simpleLabeling(
    SEPtr evaluator, Labeling::Method method) {
  Matrix matrix;
  return labeling(evaluator->sequence(), matrix, method);
}

Labeling GeneralizedHiddenMarkovModel::cachedLabeling(
    CEPtr evaluator, Labeling::Method method) {
  switch (method) {
    case Labeling::Method::bestPath:
      return labeling(evaluator->sequence(), evaluator->cache().gamma, method);
    case Labeling::Method::posteriorDecoding:
      return labeling(evaluator->sequence(),
                      evaluator->cache().posterior_decoding,
                      method);
  }
  // TODO(igorbonadio): Throw exception!
  return Labeling();
}

double GeneralizedHiddenMarkovModel::simpleProbabilityOf(
    SEPtr evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int phase) const {
  return -HUGE; // TODO(igorbonadio)
}
double GeneralizedHiddenMarkovModel::cachedProbabilityOf(
    CEPtr evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int phase) const {
  return -HUGE; // TODO(igorbonadio)
}

double GeneralizedHiddenMarkovModel::simpleProbabilityOf(
    SEPtr evaluator,
    const Sequence& s,
    unsigned int begin,
    unsigned int end) const {
  return -HUGE; // TODO(igorbonadio)
}

}  // namespace model
}  // namespace tops
