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
#include <memory>
#include <vector>

// ToPS headers
#include "model/Range.hpp"
#include "model/Symbol.hpp"
#include "model/Probability.hpp"
#include "model/DiscreteIIDModel.hpp"
#include "model/ProbabilisticModel.hpp"
#include "model/ProbabilisticModelState.hpp"

namespace tops {
namespace model {

class GeneralizedHiddenMarkovModelState;

/**
 * @typedef GeneralizedHiddenMarkovModelStatePtr
 * @brief Alias of pointer to GeneralizedHiddenMarkovModelState.
 */
using GeneralizedHiddenMarkovModelStatePtr
  = std::shared_ptr<GeneralizedHiddenMarkovModelState>;

/**
 * @class GeneralizedHiddenMarkovModelState
 * @brief TODO
 */
class GeneralizedHiddenMarkovModelState
    : public virtual ProbabilisticModelState {
 public:
  // Purely virtual methods
  virtual Probability durationProbability(unsigned int length) const = 0;
  virtual unsigned int maximumDurationSize() const = 0;
  virtual RangePtr durations() const = 0;

  virtual Id id() const = 0;
  virtual ProbabilisticModelPtr observation() = 0;
  virtual DiscreteIIDModelPtr transition() = 0;

  virtual void addPredecessor(Id id) = 0;
  virtual std::vector<Id>& predecessors() = 0;
  virtual const std::vector<Id>& predecessors() const = 0;

  virtual void addSuccessor(Id id) = 0;
  virtual std::vector<Id>& successors() = 0;
  virtual const std::vector<Id>& successors() const = 0;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_HIDDEN_MARKOV_MODEL_STATE_
