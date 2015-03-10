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
#include "model/GeneralizedHiddenMarkovModelExplicityDurationState.hpp"
#include "model/Random.hpp"
#include "model/Util.hpp"

namespace tops {
namespace model {

GeneralizedHiddenMarkovModelExplicitDurationState::GeneralizedHiddenMarkovModelExplicitDurationState() {
  setStart(0);
  setStop(0);
}

GeneralizedHiddenMarkovModelExplicitDurationState::GeneralizedHiddenMarkovModelExplicitDurationState(
    ProbabilisticModelPtr observation,
    DiscreteIIDModelPtr transition,
    Symbol name) : GeneralizedHiddenMarkovModelState(observation, transition, name) {
};

void GeneralizedHiddenMarkovModelExplicitDurationState::setDuration(ProbabilisticModelPtr d) {
  // TODO(igorbonadio)
  // _duration = d;
  // ProbabilisticModelParameters p = d->parameters();
  // ProbabilisticModelParameterValuePtr par = p.getOptionalParameterValue("number_of_phases");
  // if(par != NULL)
  //     _number_of_phases = par->getInt();
  // else
  //     _number_of_phases = 1;
}

ProbabilisticModelPtr GeneralizedHiddenMarkovModelExplicitDurationState::duration() const {
 return _duration;
}

int GeneralizedHiddenMarkovModelExplicitDurationState::chooseDuration() const {
  // TODO(igorbonadio)
  return 0.0;//_duration->choose();
}
 bool GeneralizedHiddenMarkovModelExplicitDurationState::isGeometricDuration() const {
  return false;
}

double GeneralizedHiddenMarkovModelExplicitDurationState::duration_probability(int l) const {
  // TODO(igorbonadio)
  return 0;//_duration->probabilityOf(l);
}

void GeneralizedHiddenMarkovModelExplicitDurationState::choosePredecessor (Matrix & alpha, int base, int & state, int & position, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states) {
      double sum = 0;
      double random = generateRandomDouble();
      int diff  = 0;
      if(_number_of_phases  > 1)
          diff = mod(getOutputPhase() - getInputPhase(),_number_of_phases);
      if(_number_of_phases <= 0)
          _number_of_phases = 1;
      // TODO(igorbonadio)
      int offset = 0; //duration()->size();

      if(offset > 15000)
          offset = 15000;
      int minbase = (base - diff - offset) ;
      if(minbase < 0) minbase = 0;

      for (int d = base - diff; d > minbase; d-=_number_of_phases)
          {
              position = d - 1;
              for(int k  = 0; k < (int)predecessors().size(); k++)
                  {
                      int choosed = predecessors()[k];
                      sum += exp(alpha[choosed][position] + all_states[choosed]->transition()->probabilityOf(id()) + observation()->evaluateWithPrefixSumArray(d, base, getInputPhase())  - alpha[id()][base] + duration_probability(base - d + 1 ) ) ;
                      if(sum >= random){
                          state = choosed;
                          return;
                      }
                  }
          }
  }

  void GeneralizedHiddenMarkovModelExplicitDurationState::findBestPredecessor (Matrix & gamma, Matrix &psi, Matrix &psilen, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::map < int, std::list<int> >  & valid_positions){
      int diff = 0;
      if(_number_of_phases  > 1)
          diff = mod(getOutputPhase() - getInputPhase(),_number_of_phases);
      if(_number_of_phases <= 0)
          _number_of_phases = 1;
        // TODO(igorbonadio)
      int offset = 0;//duration()->size();
      if(offset > 15000)
          offset = 15000;


      bool toContinue = false;

      for(int suc = 0; suc < (int)successors().size(); suc++)
          {
              if(!all_states[successors()[suc]]->isGeometricDuration()){
                  toContinue = true;
                  break;
              }
              if((base + 1 < (int)s.size()) && (all_states[successors()[suc]]->observation()->evaluateWithPrefixSumArray(base+1, base+1) > -HUGE)) {
                  toContinue = true;
                  break;
              }
          }
      if(!toContinue){
          return;
      }

      std::list<int>::iterator it;
      it =  (valid_positions.find(id())->second).begin();
      //      std::cerr << base << " " << all_states[id()]->name() << " " << (valid_positions.find(id())->second).size() << std::endl;
      while(it != (valid_positions.find(id())->second).end()) {
          int d = (*it)+ 1;
          if(predecessors().size() <= 0)
            return;
          int from = predecessors()[0];
          if((base - d ) < 0)
            return;
          if(base-d+1 >= offset)
              {
                  it = (valid_positions.find(id())->second).erase(it);
                  continue;
              }
          if(duration_probability(base-d+1) <= -HUGE) {
            it++;
            continue;
          }

          double emission = observation()->evaluateWithPrefixSumArray(d, base, getInputPhase());
          if(emission <= -HUGE) {
              it = (valid_positions.find(id())->second).erase(it);
              continue;
          }

    // check if it can emmit the current state given the boundaries
    int nphase = getInputPhase();
    // TODO(igorbonadio)
    // if(observation()->inhomogeneous() != NULL)
    //   nphase = observation()->inhomogeneous()->maximumTimeValue() + 1;
    if(getStart() > 0 && getStop() > 0) {
      if((d-getStart() >= 0) && (base + getStop () < s.size())) {
  double joinable = observation()->evaluateWithPrefixSumArray(d-getStart(),base+getStop(), mod(getInputPhase()-getStart(), nphase));
  //std::cerr << all_states[id()]->name() << " " << base << " " << d-getStart() << " " << base + getStop() << " " << getInputPhase() << " " << mod(getInputPhase() - getStart(), nphase) << " " << nphase << std::endl;
  if(joinable <= -HUGE) {
    it++;
    continue;
  }
      }
    }
          double gmax = gamma[from][d-1] + all_states[from]->transition()->probabilityOf(id());
          int pmax = from;
          for (int p = 1; p < (int)predecessors().size();p++){
              int from = predecessors()[p];
              double g = gamma[from][d-1] + all_states[from]->transition()->probabilityOf(id());
              if(gmax < g){
                  gmax = g;
                  pmax = from;
              }
          }

          gmax = gmax +  duration_probability(base-d+1) + observation()->evaluateWithPrefixSumArray(d, base, getInputPhase());

          if(gamma[id()][base] < gmax){
              gamma[id()][base] = gmax;
              psi[id()][base] = pmax;
              psilen[id()][base] = base-d+1;
          }
          it++;
      }
  }

double GeneralizedHiddenMarkovModelExplicitDurationState::backwardSum(Matrix &beta, const Sequence &s, int base, std::vector< std::list<int> > &valid_positions){
    int diff = 0;
    if(_number_of_phases  > 1)
      diff = mod(getOutputPhase() - getInputPhase(),_number_of_phases);
    if(_number_of_phases <= 0)
      _number_of_phases = 1;
    // TODO(igorbonadio)
    int offset = 0;//_duration->size();
    if(offset > 15000)
      offset = 15000;
    int maxbase = (base + diff + offset) ;
    if(maxbase > (int)s.size()-1) maxbase = s.size()-1;
    int phase = getInputPhase();

    std::list<int>::iterator it;
    it = valid_positions[id()].begin();
    double sum = -HUGE;
    while(it != valid_positions[id()].end()){
      if((*it) > maxbase){
  it = valid_positions[id()].erase(it);
  continue;
      }
      if((*it) < base + diff){
  it++;
  continue;
      }
      double duration = duration_probability((*it)-base);
      if(duration <= -2e20){
  it++;
  continue;
      }
  // TODO(igorbonadio)
  // if(_observation->inhomogeneous() != NULL)
  //   phase = _observation->inhomogeneous()->maximumTimeValue() + 1;
      if(getStart() > 0 && getStop() > 0) {
  if((base+1-getStart() >= 0) && ((*it) + getStop() < s.size())) {
    double joinable = _observation->evaluateWithPrefixSumArray(base+1-getStart(),(*it)+getStop(), mod(getInputPhase()-getStart(), phase));
    if(joinable <= -2e20) {
      it = valid_positions[id()].erase(it);
      continue;
    }
  }
      }
      sum = log_sum(sum, _observation->evaluateWithPrefixSumArray(base+1,(*it),getInputPhase()) + duration + beta[id()][*it]);
      it++;
    }
    return sum;
  }

void GeneralizedHiddenMarkovModelExplicitDurationState::forwardSum (Matrix & alpha, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::vector< std::list<int> > &valid_positions){
    alpha[id()][base] = -HUGE;
    if(predecessors().size() <= 0)
      return;
    int diff = 0;
    if(_number_of_phases  > 1)
      diff = mod(getOutputPhase() - getInputPhase(),_number_of_phases);
    if(_number_of_phases <= 0)
      _number_of_phases = 1;
    // TODO(igorbonadio)
    int offset = 0;//duration()->size();
    if(offset > 15000)
      offset = 15000;
    int minbase = (base - diff - offset) ;
    if(minbase < 0) minbase = 0;
    int phase = getInputPhase();

    std::list<int>::iterator it;
    it = valid_positions[id()].begin();
    while(it != valid_positions[id()].end()){
      if((*it) < minbase){
  it = valid_positions[id()].erase(it);
  continue;
      }
      if((*it) > base - diff){
  it++;
  continue;
      }
      double duration = duration_probability(base-(*it));
      if(duration <= -HUGE){
  it++;
  continue;
      }
      // TODO(igorbonadio)
  //     if(observation()->inhomogeneous() != NULL)
  // phase = observation()->inhomogeneous()->maximumTimeValue() + 1;
      if(getStart() > 0 && getStop() > 0) {
  if(((*it)+1-getStart() >= 0) && (base + getStop() < (int)s.size())) {
    double joinable = observation()->evaluateWithPrefixSumArray((*it)+1-getStart(),base+getStop(), mod(getInputPhase()-getStart(), phase));
    if(joinable <= -HUGE) {
      it = valid_positions[id()].erase(it);
      continue;
    }
  }
      }
      int from = predecessors()[0];
      double emission = observation()->evaluateWithPrefixSumArray((*it)+1, base, getInputPhase());
      alpha[id()][base] = log_sum(alpha[from][*it] + all_states[from]->transition()->probabilityOf(id()) + duration + emission, alpha[id()][base]);
      for (int k = 1; k < (int)predecessors().size(); k++){
    from = predecessors()[k];
    alpha[id()][base] =  log_sum(alpha[from][*it] + all_states[from]->transition()->probabilityOf(id()) + duration + emission, alpha[id()][base]);
      }
      it++;
    }
  }

  void GeneralizedHiddenMarkovModelExplicitDurationState::posteriorSum (Matrix & alpha, Matrix &beta, Matrix &postProbs, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::vector< std::list<int> > &valid_positions, double prob, int stateNumber){
    alpha[id()][base] = -HUGE;
    if(predecessors().size() <= 0)
      return;
    int diff = 0;
    if(_number_of_phases  > 1)
      diff = mod(getOutputPhase() - getInputPhase(),_number_of_phases);
    if(_number_of_phases <= 0)
      _number_of_phases = 1;
    // TODO(igorbonadio)
    int offset = 0;//duration()->size();
    if(offset > 15000)
      offset = 15000;
    int minbase = (base - diff - offset) ;
    if(minbase < 0) minbase = 0;
    int phase = getInputPhase();

    std::list<int>::iterator it;
    it = valid_positions[id()].begin();
    while(it != valid_positions[id()].end()){
      if((*it) < minbase){
  it = valid_positions[id()].erase(it);
  continue;
      }
      if((*it) > base - diff){
  it++;
  continue;
      }
      double duration = duration_probability(base-(*it));
      if(duration <= -HUGE){
  it++;
  continue;
      }
      // TODO(igorbonadio)
  //     if(observation()->inhomogeneous() != NULL)
  // phase = observation()->inhomogeneous()->maximumTimeValue() + 1;
      if(getStart() > 0 && getStop() > 0) {
  if(((*it)+1-getStart() >= 0) && (base + getStop() < (int)s.size())) {
    double joinable = observation()->evaluateWithPrefixSumArray((*it)+1-getStart(),base+getStop(), mod(getInputPhase()-getStart(), phase));
    if(joinable <= -HUGE) {
      it = valid_positions[id()].erase(it);
      continue;
    }
  }
      }
      double emission = observation()->evaluateWithPrefixSumArray((*it)+1, base, getInputPhase());
      for (int k = 0; k < (int)predecessors().size(); k++){
  int from = predecessors()[k];
  double w = alpha[from][*it] + all_states[from]->transition()->probabilityOf(id()) + duration + emission;
  alpha[id()][base] = log_sum(w, alpha[id()][base]);
  if(w > -HUGE && stateNumber == -1){
    int c = 0;
    float pp = exp(w + beta[id()][base] - prob);
    for(int i = (*it)+1; i <= base; i++){
      postProbs[i][classes()[c]] += pp;
      c++;
      if(c == (int)classes().size())
        c = 0;
    }
  }
  else if(w > -HUGE && stateNumber != -1){
    float pp = exp(w + beta[id()][base] - prob);
    for(int i = (*it)+1; i <= base; i++){
      postProbs[i][id()] += pp;
    }
  }
      }
      it++;
    }
  }

void GeneralizedHiddenMarkovModelExplicitDurationState::fixTransitionDistribution () const {
  // TODO(igorbonadio)
//   DiscreteIIDModelPtr trans = transition();
//   DoubleVector probabilities = (trans->parameters()).getMandatoryParameterValue("probabilities")->getDoubleVector();
//   int j = id();
//   if(probabilities.size() <= 0) {
//     return;
//   }
//   probabilities[j] = 0.0;
//   double sum = 0.0;
//   for(int i = 0; i < (int)probabilities.size(); i++)
//     {
//       if (i == j)
//         continue;
//       sum += probabilities[i];
//     }
//   for(int i = 0; i <(int) probabilities.size(); i++)
//     {
//       if (i == j)
//         continue;
//       probabilities[i]  = probabilities[i]/sum;
//     }
//   trans->setProbabilities(probabilities);
}

}  // namespace model
}  // namespace tops
