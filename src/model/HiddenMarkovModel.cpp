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
#include "model/HiddenMarkovModel.hpp"
#include "model/Util.hpp"

namespace tops {
namespace model {

HiddenMarkovModelPtr HiddenMarkovModel::make(
    std::vector<HiddenMarkovModelStatePtr> states,
    DiscreteIIDModelPtr initial_probability,
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size) {
  return HiddenMarkovModelPtr(new HiddenMarkovModel(
    states,
    initial_probability,
    state_alphabet_size,
    observation_alphabet_size));
}

HiddenMarkovModel::HiddenMarkovModel(
    std::vector<HiddenMarkovModelStatePtr> states,
    DiscreteIIDModelPtr initial_probabilities,
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size)
    : _states(states),
      _initial_probabilities(initial_probabilities),
      _state_alphabet_size(state_alphabet_size),
      _observation_alphabet_size(observation_alphabet_size) {
}

HiddenMarkovModelPtr HiddenMarkovModel::trainML(
    std::vector<Sequence> observation_training_set,
    std::vector<Sequence> state_training_set,
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size,
    double pseudocont) {
  std::vector<double> initial_probabilities(state_alphabet_size, pseudocont);
  Matrix emissions(state_alphabet_size,
                   std::vector<double>(observation_alphabet_size, pseudocont));
  Matrix transitions(state_alphabet_size,
                     std::vector<double>(state_alphabet_size, pseudocont));

  for (unsigned int i = 0; i < observation_training_set.size(); i++) {
    initial_probabilities[state_training_set[i][0]] += 1.0;
    for (unsigned int j = 0; j < observation_training_set[i].size(); j++) {
      emissions[state_training_set[i][j]][observation_training_set[i][j]]
        += 1.0;
      if (j < state_training_set[i].size() - 1) {
        transitions[state_training_set[i][j]][state_training_set[i][j+1]]
          += 1.0;
      }
    }
  }

  initial_probabilities = DiscreteIIDModel::normalize(initial_probabilities);
  std::vector<HiddenMarkovModelStatePtr> states(state_alphabet_size);
  for (unsigned int i = 0; i < state_alphabet_size; i++) {
    transitions[i] = DiscreteIIDModel::normalize(transitions[i]);
    emissions[i] = DiscreteIIDModel::normalize(emissions[i]);
    states[i] = HiddenMarkovModelState::make(
      i,
      DiscreteIIDModel::make(emissions[i]),
      DiscreteIIDModel::make(transitions[i]));
  }

  return make(states,
              DiscreteIIDModel::make(initial_probabilities),
              state_alphabet_size,
              observation_alphabet_size);
}

HiddenMarkovModelPtr HiddenMarkovModel::trainBaumWelch(
    std::vector<Sequence> observation_training_set,
    HiddenMarkovModelPtr initial_model,
    unsigned int maxiterations,
    double diff_threshold) {
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

      double P = model->forward(observation_training_set[s], alpha);
      model->backward(observation_training_set[s], beta);

      double sum = alpha[0][0] + beta[0][0];
      for (unsigned int i = 1; i < state_alphabet_size; i++)
        sum = log_sum(sum, alpha[i][0] + beta[i][0]);

      for (unsigned int i = 0; i < state_alphabet_size; i++)
        pi[i] = alpha[i][0] + beta[i][0] - sum;

      for (unsigned int i = 0; i < state_alphabet_size; i++) {
        for (unsigned int j = 0; j < state_alphabet_size; j++) {
          unsigned int t = 0;
          double sum = -HUGE;
          if (t < observation_training_set[s].size()-1) {
            sum = alpha[i][t] + model->state(i)->transitions()->probabilityOf(j)
                + model->state(j)->emissions()->probabilityOf(
                    observation_training_set[s][t+1])
                + beta[j][t+1];
            for (t = 1; t < observation_training_set[s].size()-1; t++)
              sum = log_sum(sum,
                  alpha[i][t] + model->state(i)->transitions()->probabilityOf(j)
                  + model->state(j)->emissions()->probabilityOf(
                      observation_training_set[s][t+1])
                  + beta[j][t+1]);
          }
          A[i][j] = sum;
        }

        for (unsigned int sigma = 0; sigma < observation_alphabet_size;
            sigma++) {
          double sum = -HUGE;
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

      std::vector<HiddenMarkovModelStatePtr> states(state_alphabet_size);
      for (unsigned int k = 0; k < state_alphabet_size; k++) {
        for (unsigned int l = 0; l < state_alphabet_size; l++) {
          A[k][l] = A[k][l] - sumA[k];
        }
        for (unsigned int b = 0; b <observation_alphabet_size; b++) {
          E[k][b] = E[k][b] - sumE[k];
        }
        states[k] = HiddenMarkovModelState::make(
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

HiddenMarkovModelStatePtr HiddenMarkovModel::state(unsigned int i) const {
  return _states[i];
}

unsigned int HiddenMarkovModel::stateAlphabetSize() const {
  return _state_alphabet_size;
}

unsigned int HiddenMarkovModel::observationAlphabetSize() const {
  return _observation_alphabet_size;
}

// double HiddenMarkovModel::evaluateSequence(const Sequence &xs,
//                                            unsigned int begin,
//                                            unsigned int end,
//                                            unsigned int phase) const {
//   Matrix alpha;
//   forward(xs, alpha);
//   double sum_end = -HUGE;
//   double sum_begin = -HUGE;
//   for (unsigned int k = 0; k < _state_alphabet_size; k++) {
//     sum_end = log_sum(sum_end, alpha[k][end-1]);
//     if (begin != 0)
//       sum_begin = log_sum(sum_begin, alpha[k][begin-1]);
//     else
//       sum_begin = 0;
//   }
//   return sum_end - sum_begin;
// }

double HiddenMarkovModel::evaluatePosition(
    const Sequence &xs,
    unsigned int i,
    unsigned int phase) const {
  return -HUGE;
}

Symbol HiddenMarkovModel::choosePosition(const Sequence &xs,
                                         unsigned int i,
                                         unsigned int phase) const {
  // TODO(igorbonadio)
  return INVALID_SYMBOL;
}

double HiddenMarkovModel::evaluateSequencesPosition(const Sequence &xs,
                                                    const Sequence &ys,
                                                    unsigned int i) const {
  double transition;
  if (i == 0)
    transition = _initial_probabilities->probabilityOf(ys[0]);
  else
    transition = _states[ys[i-1]]->transitions()->probabilityOf(ys[i]);
  return transition + _states[ys[i]]->emissions()->probabilityOf(xs[i]);
}

double HiddenMarkovModel::evaluateSequences(const Sequence &xs,
                                            const Sequence &ys,
                                            unsigned int begin,
                                            unsigned int end) const {
  double prob = 0;
  for (unsigned int i = begin; i < end; i++)
    prob += evaluateSequencesPosition(xs, ys, i);
  return prob;
}

Labeling HiddenMarkovModel::viterbi(const Sequence &xs,
                                    Matrix &gamma) const {
  gamma = std::vector<std::vector<double>>(
      _state_alphabet_size,
      std::vector<double>(xs.size()));
  Matrix psi(_state_alphabet_size, std::vector<double>(xs.size()));

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    gamma[k][0] = _initial_probabilities->probabilityOf(k)
        + _states[k]->emissions()->probabilityOf(xs[0]);

  for (unsigned int i = 0; i < xs.size() - 1; i++) {
    for (unsigned int k = 0; k < _state_alphabet_size; k++) {
      gamma[k][i+1] =  gamma[0][i]
          + _states[0]->transitions()->probabilityOf(k);
      psi[k][i+1] = 0;
      for (unsigned int p = 1; p < _state_alphabet_size; p++) {
        double v = gamma[p][i] + _states[p]->transitions()->probabilityOf(k);
        if (gamma[k][i+1] < v) {
          gamma[k][i+1] = v;
          psi[k][i+1] = p;
        }
      }
      gamma[k][i+1] += _states[k]->emissions()->probabilityOf(xs[i+1]);
    }
  }

  Sequence ys(xs.size());
  ys[xs.size() - 1] = 0;
  double max = gamma[0][xs.size() - 1];
  for (unsigned int k = 1; k < _state_alphabet_size; k++) {
    if (max < gamma[k][xs.size() - 1]) {
      max = gamma[k][xs.size() - 1];
      ys[xs.size() - 1] = k;
    }
  }
  for (int i = xs.size() - 1; i >= 1; i--) {
    ys[i-1] = psi[ys[i]][i];
  }

  return Labeling(max, std::move(ys));
}

double HiddenMarkovModel::backward(const Sequence & xs, Matrix &beta) const {
  beta = std::vector<std::vector<double>>(
      _state_alphabet_size,
      std::vector<double>(xs.size()));

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    beta[k][xs.size()-1] = 0.0;

  for (int t = xs.size()-2; t >= 0; t--) {
    for (unsigned int i = 0; i < _state_alphabet_size; i++) {
      beta[i][t] =  _states[i]->transitions()->probabilityOf(0)
          + _states[0]->emissions()->probabilityOf(xs[t+1]) + beta[0][t+1];
      for (unsigned int j = 1; j < _state_alphabet_size; j++) {
        beta[i][t] = log_sum(
            beta[i][t],
            _states[i]->transitions()->probabilityOf(j)
                + _states[j]->emissions()->probabilityOf(xs[t+1])
                + beta[j][t+1]);
      }
    }
  }

  double sum = beta[0][0] + _initial_probabilities->probabilityOf(0)
      + _states[0]->emissions()->probabilityOf(xs[0]);
  for (unsigned int k = 1; k < _state_alphabet_size; k++) {
    sum = log_sum(sum, beta[k][0] + _initial_probabilities->probabilityOf(k)
        + _states[k]->emissions()->probabilityOf(xs[0]));
  }

  return sum;
}

double HiddenMarkovModel::forward(const Sequence & xs, Matrix &alpha) const {
  alpha = std::vector<std::vector<double>>(
      _state_alphabet_size,
      std::vector<double>(xs.size()));

  for (unsigned int k = 0; k < _state_alphabet_size; k++)
    alpha[k][0] = _initial_probabilities->probabilityOf(k)
        + _states[k]->emissions()->probabilityOf(xs[0]);

  for (unsigned int t = 0; t < xs.size() - 1; t++) {
    for (unsigned int i = 0; i < _state_alphabet_size; i++) {
      alpha[i][t+1] = alpha[0][t] + _states[0]->transitions()->probabilityOf(i);
      for (unsigned int j = 1; j < _state_alphabet_size; j++) {
        alpha[i][t+1] = log_sum(
            alpha[i][t+1],
            alpha[j][t] + _states[j]->transitions()->probabilityOf(i));
      }
      alpha[i][t+1] += _states[i]->emissions()->probabilityOf(xs[t+1]);
    }
  }

  double sum =  alpha[0][xs.size()-1];
  for (unsigned int k = 1; k < _state_alphabet_size; k++) {
    sum = log_sum(sum, alpha[k][xs.size()-1]);
  }
  return sum;
}

void HiddenMarkovModel::posteriorProbabilities(const Sequence & xs,
                                               Matrix & probabilities) const {
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

Labeling HiddenMarkovModel::posteriorDecoding(const Sequence &xs,
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
  return Labeling(evaluateSequences(xs, path, 0, xs.size()), std::move(path));
}

void HiddenMarkovModel::chooseSequences(Sequence &xs,
                                        Sequence &ys,
                                        unsigned int size) const {
  xs.resize(size);
  ys.resize(size);
  for (unsigned int i = 0; i < size; i++)
    chooseSequencesPosition(xs, ys, i);
}

void HiddenMarkovModel::chooseSequencesPosition(Sequence &xs,
                                                Sequence &ys,
                                                unsigned int i) const {
  if (i == 0)
    ys[i] = _initial_probabilities->choose();
  else
    ys[i] = _states[ys[i-1]]->transitions()->choose();
  xs[i] = _states[ys[i]]->emissions()->choose();
}

}  // namespace model
}  // namespace tops
