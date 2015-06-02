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

#ifndef TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_EXPLICIT_DURATION_STATE_
#define TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_EXPLICIT_DURATION_STATE_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "GeneralizedHiddenMarkovModelState.hpp"
#include "ProbabilisticModel.hpp"
#include "DiscreteIIDModel.hpp"

namespace tops {
namespace model {

class GeneralizedHiddenMarkovModelExplicitDurationState;

/**
 * @typedef GeneralizedHiddenMarkovModelExplicitDurationStatePtr
 * @brief Alias of pointer to GeneralizedHiddenMarkovModelExplicitDurationState.
 */
using GeneralizedHiddenMarkovModelExplicitDurationStatePtr = std::shared_ptr<GeneralizedHiddenMarkovModelExplicitDurationState>;

/**
 * @class GeneralizedHiddenMarkovModelExplicitDurationState
 * @brief TODO
 */
class GeneralizedHiddenMarkovModelExplicitDurationState : public GeneralizedHiddenMarkovModelState {
 public:
  static GeneralizedHiddenMarkovModelExplicitDurationStatePtr make(Symbol symbol,
                                                                   ProbabilisticModelPtr observation,
                                                                   DiscreteIIDModelPtr transition,
                                                                   DiscreteIIDModelPtr duration);

  virtual double durationProbability(int l) const;
  virtual bool isGeometricDuration() const;
  virtual int maximumDurationSize() const;
  virtual RangePtr durations() const;

 private:
  GeneralizedHiddenMarkovModelExplicitDurationState(Symbol symbol,
                                                    ProbabilisticModelPtr observation,
                                                    DiscreteIIDModelPtr transition,
                                                    DiscreteIIDModelPtr duration);

  DiscreteIIDModelPtr _duration;
  unsigned int _max_duration = 100;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_HIDDEN_MARKOV_MODEL_STATE_
