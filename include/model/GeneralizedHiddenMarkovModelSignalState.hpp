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
#include <memory>
#include <vector>

// ToPS headers
#include "GeneralizedHiddenMarkovModelState.hpp"
#include "ProbabilisticModel.hpp"
#include "DiscreteIIDModel.hpp"

namespace tops {
namespace model {

class GeneralizedHiddenMarkovModelSignalState;

/**
 * @typedef GeneralizedHiddenMarkovModelSignalStatePtr
 * @brief Alias of pointer to GeneralizedHiddenMarkovModelSignalState.
 */
using GeneralizedHiddenMarkovModelSignalStatePtr
  = std::shared_ptr<GeneralizedHiddenMarkovModelSignalState>;

/**
 * @class GeneralizedHiddenMarkovModelSignalState
 * @brief TODO
 */
class GeneralizedHiddenMarkovModelSignalState
    : public GeneralizedHiddenMarkovModelState {
 public:
  // Static methods
  static GeneralizedHiddenMarkovModelSignalStatePtr make(Symbol symbol,
                                                         ProbabilisticModelPtr observation,
                                                         DiscreteIIDModelPtr transition,
                                                         int duration_size);

  // Virtual methods
  virtual double durationProbability(int l) const;
  virtual int maximumDurationSize() const;
  virtual RangePtr durations() const;

 private:
  // Instance variables
  int _duration_size;

  // Constructors
  GeneralizedHiddenMarkovModelSignalState(Symbol symbol,
                                          ProbabilisticModelPtr observation,
                                          DiscreteIIDModelPtr transition,
                                          int duration_size);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_HIDDEN_MARKOV_MODEL_STATE_
