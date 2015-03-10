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

#ifndef TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_SIGNAL_STATE_
#define TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_SIGNAL_STATE_

// Standard headers
#include <vector>

// ToPS headers
#include "model/GeneralizedHiddenMarkovModelState.hpp"

namespace tops {
namespace model {

class GeneralizedHiddenMarkovModelSignalState: public GeneralizedHiddenMarkovModelState {
  GeneralizedHiddenMarkovModelSignalState() ;
  GeneralizedHiddenMarkovModelSignalState(
      ProbabilisticModelPtr observation,
      DiscreteIIDModelPtr transition, Symbol name);

  virtual int size() const ;
  virtual void setSize(int s);
  virtual int chooseDuration() const ;
  virtual double getThreshold() const ;
  virtual void setThreshold(double threshold) ;
  virtual double duration_probability(int l) const ;
  virtual void fixTransitionDistribution () const ;
  virtual void findBestPredecessor (Matrix & gamma, Matrix &psi, Matrix &psilen, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::map < int, std::list<int> >  & valid_positions );
  virtual void forwardSum (Matrix & alpha, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::vector< std::list<int> > &valid_positions);
  virtual double backwardSum(Matrix &beta, const Sequence &s, int base, std::vector< std::list<int> > &valid_positions);
  virtual void posteriorSum (Matrix & alpha, Matrix &beta, Matrix &postProbs, const Sequence & s, int base, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states, std::vector< std::list<int> > &valid_positions, double prob, int stateNumber);
  virtual void choosePredecessor (Matrix & alpha, int base, int & state, int & position, const std::vector<GeneralizedHiddenMarkovModelStatePtr> & all_states);

 private:
  int _size;
  double _threshold;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_SIGNAL_STATE_
