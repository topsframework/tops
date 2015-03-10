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
#include <vector>
#include <cmath>

// ToPS headers
#include "model/GeneralizedHiddenMarkovModelState.hpp"
#include "model/Random.hpp"
#include "model/Util.hpp"

namespace tops {
namespace model {

GeneralizedHiddenMarkovModelState::GeneralizedHiddenMarkovModelState() {
    _start = 0;
    _stop = 0;
    isRightJoinable(0);
    isLeftJoinable(0);
}

GeneralizedHiddenMarkovModelState::GeneralizedHiddenMarkovModelState(
    ProbabilisticModelPtr observation,
    DiscreteIIDModelPtr transition,
    Symbol name)
      : _name(name),
        _observation(observation),
        _transition(transition) {
}

void GeneralizedHiddenMarkovModelState::findBestPredecessor(Matrix & gamma, Matrix &psi, Matrix &psilen, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::map < int, std::list<int> >  & valid_positions){
    int d = 1;
    if(predecessors().size() <= 0)
        return;

    int from = _predecessors[0];
    double gmax = gamma[from][base-d] + all_states[from]->transition()->probabilityOf(id());
    int pmax = from;
    for (int p = 1; p < (int)_predecessors.size();p++){
        int from = _predecessors[p];
        double g = gamma[from][base-d] + all_states[from]->transition()->probabilityOf(id());
        if(gmax < g){
            gmax = g;
            pmax = from;
        }
    }
    int phase = getInputPhase();
    gmax = gmax + duration_probability(d) + observation()->evaluateWithPrefixSumArray(base-d +1, base, phase);

    if(gamma[id()][base] < gmax){
        gamma[id()][base] = gmax;
        psi[id()][base] = pmax;
        psilen[id()][base] = d;
    }
}

void GeneralizedHiddenMarkovModelState::choosePredecessor (Matrix & alpha, int base, int & state, int & position, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states) {
    double sum = 0;


    double random = generateRandomDouble();

    for (int k = 0; k < (int)predecessors().size(); k++)
        {
            int choosed = predecessors()[k];
            sum += std::exp(alpha[choosed][base - 1] + all_states[choosed]->transition()->probabilityOf(id()) + observation()->evaluateWithPrefixSumArray(base, base, getInputPhase()) - alpha[id()][base]);
            if(sum >= random){
                state = choosed;
                position = base - 1;
                return;
            }
        }
}

void GeneralizedHiddenMarkovModelState::setObservation(ProbabilisticModelPtr obs) {
  _observation = obs;
}

ProbabilisticModelPtr GeneralizedHiddenMarkovModelState::observation() const {
  return _observation;
}

void GeneralizedHiddenMarkovModelState::setTransition(DiscreteIIDModelPtr trans) {
  _transition = trans;
}

DiscreteIIDModelPtr GeneralizedHiddenMarkovModelState::transition() const {
  return _transition;
}

int GeneralizedHiddenMarkovModelState::chooseDuration() const {
  return 1;
}

int GeneralizedHiddenMarkovModelState::id() const {
  return _name;
}

void GeneralizedHiddenMarkovModelState::addPredecessor(int id) {
  _predecessors.push_back(id);
}

std::vector<int> & GeneralizedHiddenMarkovModelState::predecessors() {
  return _predecessors;
}

void GeneralizedHiddenMarkovModelState::addSuccessor(int id) {
  _successors.push_back(id);
}

std::vector<int> & GeneralizedHiddenMarkovModelState::successors() {
  return _successors;
}

std::vector<int> & GeneralizedHiddenMarkovModelState::classes() {
  return _classes;
}

double GeneralizedHiddenMarkovModelState::duration_probability(int l) const {
  if (l == 1)
    return 0.0;
  else
    return -HUGE;
}

bool GeneralizedHiddenMarkovModelState::isGeometricDuration() const {
  return true;
}

int GeneralizedHiddenMarkovModelState::getInputPhase() const {
  return _inputPhase;
}

void GeneralizedHiddenMarkovModelState::setInputPhase(int _inputPhase) {
  this->_inputPhase = _inputPhase;
}

int GeneralizedHiddenMarkovModelState::getStart() const {
  return _start;
}

void GeneralizedHiddenMarkovModelState::setStart(int start) {
  this->_start = start;
}

void GeneralizedHiddenMarkovModelState::setClasses(std::vector<int> &classes){
  _classes = classes;
}

int GeneralizedHiddenMarkovModelState::getStop() const {
  return _stop;
}

void GeneralizedHiddenMarkovModelState::setStop(int stop) {
  this->_stop = stop;
}

  int GeneralizedHiddenMarkovModelState::getOutputPhase() const {
  return _outputPhase;
}

void GeneralizedHiddenMarkovModelState::setOutputPhase(int _outputPhase) {
  this->_outputPhase = _outputPhase;
}

void GeneralizedHiddenMarkovModelState::forwardSum (Matrix & alpha, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::vector< std::list<int> > &valid_positions){
  alpha[id()][base] = -HUGE;
  if(predecessors().size() <= 0)
    return;
  int from = predecessors()[0];
  int phase = getInputPhase();
  double emission = observation()->evaluateWithPrefixSumArray(base, base, phase);
  alpha[id()][base] =  alpha[from][base-1] + all_states[from]->transition()->probabilityOf(id()) + emission;
  for (int k = 1; k < (int)predecessors().size(); k++) {
    from = predecessors()[k];
    alpha[id()][base] = log_sum(alpha[from][base - 1] + all_states[from]->transition()->probabilityOf(id()) +  emission, alpha[id()][base]);
  }
}

double GeneralizedHiddenMarkovModelState::backwardSum(Matrix &beta, const Sequence &s, int base, std::vector< std::list<int> > &valid_positions){
  int phase = getInputPhase();
  double result = _observation->evaluateWithPrefixSumArray(base+1, base+1, phase) + beta[id()][base+1];
  return result;
}

void GeneralizedHiddenMarkovModelState::posteriorSum (Matrix & alpha, Matrix & beta, Matrix &postProbs, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::vector< std::list<int> > &valid_positions, double prob, int stateNumber){
  alpha[id()][base] = -HUGE;
  if(predecessors().size() <= 0)
    return;
  int phase = getInputPhase();
  double emission = _observation->evaluateWithPrefixSumArray(base, base, phase);
  for (int k = 0; k < (int)predecessors().size(); k++)
    {
int from = predecessors()[k];
alpha[id()][base] =  log_sum(alpha[from][base - 1] + all_states[from]->transition()->probabilityOf(id()) +  emission, alpha[id()][base]);
    }
  if(stateNumber == -1){
    for(int c = 0; c < (int)classes().size(); c++){
      postProbs[base][classes()[c]] += exp(alpha[id()][base] + beta[id()][base] - prob);
    }
  }
  else if(stateNumber != -1)
    postProbs[base][id()] = exp(alpha[id()][base] + beta[id()][base] - prob);
}

void GeneralizedHiddenMarkovModelState::isLeftJoinable(int joinable){
  this->_left_joinable = joinable;
}

int GeneralizedHiddenMarkovModelState::isLeftJoinable() const {
  return this->_left_joinable;
}

void GeneralizedHiddenMarkovModelState::isRightJoinable(int joinable){
  this->_right_joinable = joinable;
}

int GeneralizedHiddenMarkovModelState::isRightJoinable() const {
  return this->_right_joinable;
}

void GeneralizedHiddenMarkovModelState::clearPredecessorSuccessor() {
  _successors.clear();
  _predecessors.clear();
}

}  // namespace model
}  // namespace tops
