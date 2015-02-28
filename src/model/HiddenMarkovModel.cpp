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
          _state_alphabet_size(state_alphabet_size) {

}

double HiddenMarkovModel::evaluatePosition(const Sequence &xs, unsigned int i) const {
  // TODO(igorbonadio)
  return 0.0;
}

Symbol HiddenMarkovModel::choosePosition(const Sequence &xs, unsigned int i) const {
  // TODO(igorbonadio)
  return INVALID_SYMBOL;
}

double HiddenMarkovModel::evaluateSequencesPosition(const Sequence &xs, const Sequence &ys, unsigned int i) const {
  double transition;
  if (i == 0)
    transition = _initial_probabilities->probabilityOf(ys[0]);
  else
    transition = _states[ys[i-1]]->transitions()->probabilityOf(ys[i]);
  return transition + _states[ys[i]]->emissions()->probabilityOf(xs[i]);
}

double HiddenMarkovModel::evaluateSequences(const Sequence &xs, const Sequence &ys, unsigned int begin, unsigned int end) const {
  double prob = 0;
  for (unsigned int i = begin; i < end; i++)
    prob += evaluateSequencesPosition(xs, ys, i);
  return prob;
}

double HiddenMarkovModel::viterbi (const Sequence &xs, Sequence &ys, Matrix &gamma) const {
  gamma = std::vector<std::vector<double>>(_state_alphabet_size, std::vector<double>(xs.size()));
  Matrix psi(_state_alphabet_size, std::vector<double>(xs.size()));

  for(unsigned int k = 0; k < _state_alphabet_size; k++)
    gamma[k][0] = _initial_probabilities->probabilityOf(k) + _states[k]->emissions()->probabilityOf(xs[0]);

  for (unsigned int i = 0; i < xs.size() - 1; i++) {
    for (unsigned int k = 0; k < _state_alphabet_size; k++) {
      gamma[k][i+1] =  gamma[0][i] + _states[0]->transitions()->probabilityOf(k);
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

  ys.resize(xs.size());
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

  return max;
}

double HiddenMarkovModel::backward(const Sequence & xs, Matrix &beta) const {
  beta = std::vector<std::vector<double>>(_state_alphabet_size, std::vector<double>(xs.size()));

  for(unsigned int k = 0; k < _state_alphabet_size; k++)
    beta[k][xs.size()-1] = 0.0;

  for (int t = xs.size()-2; t >= 0; t--) {
    for(unsigned int i = 0; i < _state_alphabet_size; i++) {
      beta[i][t] =  _states[i]->transitions()->probabilityOf(0) + _states[0]->emissions()->probabilityOf(xs[t+1]) + beta[0][t+1];
      for(unsigned int j = 1; j < _state_alphabet_size; j++) {
        beta[i][t] = log_sum(beta[i][t], _states[i]->transitions()->probabilityOf(j) + _states[j]->emissions()->probabilityOf(xs[t+1]) + beta[j][t+1]);
      }
    }
  }

  double sum = beta[0][0] + _initial_probabilities->probabilityOf(0) + _states[0]->emissions()->probabilityOf(xs[0]);
  for(unsigned int k = 1; k < _state_alphabet_size; k++) {
    sum = log_sum(sum, beta[k][0] + _initial_probabilities->probabilityOf(k) + _states[k]->emissions()->probabilityOf(xs[0]));
  }

  return sum;
}

double HiddenMarkovModel::forward(const Sequence & xs, Matrix &alpha) const {
  alpha = std::vector<std::vector<double>>(_state_alphabet_size, std::vector<double>(xs.size()));

  for(unsigned int k = 0; k < _state_alphabet_size; k++)
    alpha[k][0] = _initial_probabilities->probabilityOf(k) + _states[k]->emissions()->probabilityOf(xs[0]);

  for (unsigned int t = 0; t < xs.size() - 1; t++) {
    for (unsigned int i = 0; i < _state_alphabet_size; i++) {
      alpha[i][t+1] =  alpha[0][t] + _states[0]->transitions()->probabilityOf(i);
      for (unsigned int j = 1; j < _state_alphabet_size; j++) {
        alpha[i][t+1] = log_sum(alpha[i][t+1], alpha[j][t] + _states[j]->transitions()->probabilityOf(i));
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

void HiddenMarkovModel::posteriorProbabilities(const Sequence & xs, Matrix & probabilities) const {
  probabilities = std::vector<std::vector<double>>(_state_alphabet_size, std::vector<double>(xs.size()));

  Matrix alpha; // forward
  Matrix beta;  // backward

  double full = forward(xs, alpha);
  backward(xs, beta);

  for(unsigned int k = 0; k < _state_alphabet_size; k++)
    for(unsigned int i = 0; i < xs.size(); i++)
      probabilities[k][i] = alpha[k][i] + beta[k][i] - full;
}

void HiddenMarkovModel::posteriorDecoding(const Sequence & xs, Sequence & path, Matrix & probabilities) const {
  posteriorProbabilities(xs, probabilities);

  path.resize(xs.size());

  double max;
  for (unsigned int i = 0; i < xs.size(); i++) {
    max = probabilities[0][i];
    path[i] = 0;
    for (unsigned int k = 1; k < _state_alphabet_size; k++) {
      if (probabilities[k][i] > max) {
        max = probabilities[k][i];
        path[i] = k;
      }
    }
  }
}

void HiddenMarkovModel::chooseSequences(Sequence &xs, Sequence &ys, unsigned int size) const {
  xs.resize(size);
  ys.resize(size);
  for (unsigned int i = 0; i < size; i++)
    chooseSequencesPosition(xs, ys, i);
}

void HiddenMarkovModel::chooseSequencesPosition(Sequence &xs, Sequence &ys, unsigned int i) const {
  if (i == 0)
    ys[i] = _initial_probabilities->choose();
  else
    ys[i] = _states[ys[i-1]]->transitions()->choose();
  xs[i] = _states[ys[i]]->emissions()->choose();
}

}  // namespace model
}  // namespace tops
