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
#include "model/GeneralizedHiddenMarkovModelSignalState.hpp"
#include "model/Random.hpp"
#include "model/Util.hpp"

namespace tops {
namespace model {

GeneralizedHiddenMarkovModelSignalState::GeneralizedHiddenMarkovModelSignalState() {
  setStart(0);
  setStop(0);
  isRightJoinable(0);
  isLeftJoinable(0);
}

GeneralizedHiddenMarkovModelSignalState::GeneralizedHiddenMarkovModelSignalState(
    ProbabilisticModelPtr observation,
    DiscreteIIDModelPtr transition,
    Symbol name)
      : GeneralizedHiddenMarkovModelState(observation, transition, name) {
};

int GeneralizedHiddenMarkovModelSignalState::size() const {
  return _size;
}
void GeneralizedHiddenMarkovModelSignalState::setSize(int s) {
  _size = s;
}

int GeneralizedHiddenMarkovModelSignalState::chooseDuration() const {
  return _size;
}

double GeneralizedHiddenMarkovModelSignalState::getThreshold() const {
  return _threshold;
}

void GeneralizedHiddenMarkovModelSignalState::setThreshold(double threshold) {
  _threshold = threshold;
}

double GeneralizedHiddenMarkovModelSignalState::duration_probability(int l) const {
  if (l == _size)
      return 0.0;
  else
    return -HUGE;
}

void GeneralizedHiddenMarkovModelSignalState::findBestPredecessor (Matrix & gamma, Matrix &psi, Matrix &psilen, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::map < int, std::list<int> >  & valid_positions){
    int d = size();
    if(predecessors().size() <= 0)
        return;

    int from = predecessors()[0];
    if((base - d ) < 0)
        return;
    double gmax = gamma[from][base-d] + all_states[from]->transition()->probabilityOf(id());
    int pmax = from;
    for (int p = 1; p < (int)predecessors().size();p++){
        int from = predecessors()[p];
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

void GeneralizedHiddenMarkovModelSignalState::choosePredecessor (Matrix & alpha, int base, int & state, int & position, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states) {
    double sum = 0;
    double random = generateRandomDouble();
    position = base - size() ;
    if(position < 0)
        position = 0;
    for(int k  = 0; k < (int)predecessors().size(); k++)
        {
            int choosed = predecessors()[k];
            sum += exp(alpha[choosed][position] + all_states[choosed]->transition()->probabilityOf(id()) + observation()->evaluateWithPrefixSumArray(position + 1, base, getInputPhase())  - alpha[id()][base]+ duration_probability(base - position) ) ;
            if(sum >= random){
                state = choosed;
                return;
            }
        }
}

void GeneralizedHiddenMarkovModelSignalState::fixTransitionDistribution () const {
  // TODO(igorbonadio)
  // DiscreteIIDModelPtr trans = transition();
  // DoubleVector probabilities = (trans->parameters()).getMandatoryParameterValue("probabilities")->getDoubleVector();
  // int j = id();
  // if(probabilities.size() <= 0) {
  //   return;
  // }
  // probabilities[j] = 0.0;
  // double sum = 0.0;
  // for(int i = 0; i < (int)probabilities.size(); i++)
  //   {
  //       if (i == j)
  //           continue;
  //       sum += probabilities[i];
  //   }
  // for(int i = 0; i < (int)probabilities.size(); i++)
  //   {
  //       if (i == j)
  //           continue;
  //       probabilities[i]  = probabilities[i]/sum;
  //   }
  // trans->setProbabilities(probabilities);
}

double GeneralizedHiddenMarkovModelSignalState::backwardSum(Matrix &beta, const Sequence &s, int base, std::vector< std::list<int> > &valid_positions){
  int d = size();
  if(base+d >= (int)s.size())
    return -HUGE;
  int phase = getInputPhase();
  double result = _observation->evaluateWithPrefixSumArray(base+1,base+d,phase) + beta[id()][base+d];
  return result;
}

void GeneralizedHiddenMarkovModelSignalState::forwardSum (Matrix & alpha, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::vector< std::list<int> > &valid_positions){
  alpha[id()][base]  = -HUGE;
  int d = size();
  if(predecessors().size() <= 0)
    return;

  int from = predecessors()[0];
  if((base - d ) < 0)
    return;
  int phase = getInputPhase();
  double emission = observation()->evaluateWithPrefixSumArray(base - d+ 1, base, phase);

  alpha[id()][base] =  alpha[from][base-d] + all_states[from]->transition()->probabilityOf(id())  + emission;
  for (int k = 1; k < (int)predecessors().size(); k++)
    {
from = predecessors()[k];
alpha[id()][base] =  log_sum(alpha[from][base -d] + all_states[from]->transition()->probabilityOf(id()) + emission, alpha[id()][base]);
    }
  if(alpha[id()][base] > -HUGE){
    std::vector<int> succ = successors();
    for(int p = 0; p < (int)succ.size(); p++){
int id = succ[p];
if(!all_states[id]->isGeometricDuration())
  valid_positions[id].push_front(base);
    }
  }
}

void GeneralizedHiddenMarkovModelSignalState::posteriorSum (Matrix & alpha, Matrix & beta, Matrix &postProbs, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::vector< std::list<int> > &valid_positions, double prob, int stateNumber){
  alpha[id()][base]  = -HUGE;
  int d = size();
  if(predecessors().size() <= 0)
    return;

  if((base - d ) < 0)
    return;
  int phase = getInputPhase();
  double emission = _observation->evaluateWithPrefixSumArray(base - d+ 1, base, phase);
  if(emission <= -HUGE)
    return;

  for (int k = 0; k < (int)predecessors().size(); k++)
    {
int from = predecessors()[k];
double w = alpha[from][base -d] + all_states[from]->transition()->probabilityOf(id()) + emission;
alpha[id()][base] =  log_sum(w, alpha[id()][base]);
if(w > -HUGE && stateNumber == -1){
  int c = 0;
  float pp = exp(w + beta[id()][base] - prob);
  for(int i = base-d+1; i <= base; i++){
    postProbs[i][classes()[c]] += pp;
    c++;
    if(c == (int)classes().size())
      c = 0;
  }
}
else if(w > -HUGE && stateNumber != -1){
  float pp = exp(w + beta[id()][base] - prob);
  for(int i = base-d+1; i <= base; i++){
    postProbs[i][id()] += pp;
  }
}
    }
  if(alpha[id()][base] > -HUGE){
    std::vector<int> succ = successors();
    for(int p = 0; p < (int)succ.size(); p++){
int id = succ[p];
if(!all_states[id]->isGeometricDuration())
  valid_positions[id].push_front(base);
    }
  }
}

}  // namespace model
}  // namespace tops
