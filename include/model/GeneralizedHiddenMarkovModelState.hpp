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

#ifndef TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_STATE_
#define TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_STATE_

// Standard headers
#include <vector>
#include <map>
#include <list>
#include <string>
#include <memory>

// ToPS headers
#include "ProbabilisticModel.hpp"
#include "DiscreteIIDModel.hpp"
#include "Symbol.hpp"
#include "Matrix.hpp"

namespace tops {
namespace model {

class GeneralizedHiddenMarkovModelState;

using GeneralizedHiddenMarkovModelStatePtr = std::shared_ptr<GeneralizedHiddenMarkovModelState>;

class GeneralizedHiddenMarkovModelState {
 public:
  GeneralizedHiddenMarkovModelState();
  GeneralizedHiddenMarkovModelState(ProbabilisticModelPtr observation, DiscreteIIDModelPtr transition, Symbol name);

  virtual void setObservation(ProbabilisticModelPtr obs) ;
  virtual ProbabilisticModelPtr observation() const ;
  virtual void setTransition(DiscreteIIDModelPtr trans) ;
  virtual DiscreteIIDModelPtr transition() const ;
  virtual int chooseDuration() const ;
  virtual int id() const ;
  virtual void addPredecessor(int id) ;
  virtual std::vector<int> & predecessors() ;
  virtual void addSuccessor(int id) ;
  virtual void clearPredecessorSuccessor();
  virtual std::vector<int> & successors() ;
  virtual std::vector<int> & classes() ;
  virtual void setClasses(std::vector<int> &classes);
  virtual double duration_probability(int l) const ;
  virtual bool isGeometricDuration() const ;
  virtual int getInputPhase() const ;
  virtual void setInputPhase(int _inputPhase) ;
  virtual int getOutputPhase() const ;
  virtual void setOutputPhase(int _outputPhase) ;
  virtual int getStart() const ;
  virtual void setStart(int start) ;
  virtual int getStop() const ;
  virtual void setStop(int stop) ;
  virtual void isLeftJoinable(int joinable);
  virtual int isLeftJoinable() const;
  virtual void isRightJoinable(int joinable);
  virtual int isRightJoinable() const;
  virtual void fixTransitionDistribution () const {} ;
  virtual void findBestPredecessor (Matrix & gamma, Matrix &psi, Matrix &psilen, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::map < int, std::list<int> >  & valid_positions );
  virtual void forwardSum (Matrix & alpha, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::vector< std::list<int> > &valid_positions);
  virtual double backwardSum(Matrix &beta, const Sequence &s, int base, std::vector< std::list<int> > &valid_positions);
  virtual void posteriorSum (Matrix & alpha, Matrix &beta, Matrix &postProbs, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::vector< std::list<int> > &valid_positions, double prob, int stateNumber);
  virtual void choosePredecessor (Matrix & alpha, int base, int & state, int & position , const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states);
 protected:
  Symbol _name;
  ProbabilisticModelPtr _observation;
  DiscreteIIDModelPtr _transition;
  std::vector<int> _predecessors;
  std::vector<int> _successors;
  std::vector<int> _classes;
  int _inputPhase;
  int _outputPhase;
  int _start;
  int _stop;
  bool _left_joinable;
  bool _right_joinable;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_STATE_
