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

#ifndef TOPS_MODEL_GHMM_SIGNAL_DURATION_STATE_
#define TOPS_MODEL_GHMM_SIGNAL_DURATION_STATE_

// Standard headers
#include <memory>
#include <vector>

// ToPS templates
#include "model/GHMMStateCrtp.tcc"

namespace tops {
namespace model {

class GHMMSignalDurationState;

/**
 * @typedef GHMMSignalDurationStatePtr
 * @brief Alias of pointer to GHMMSignalDurationState.
 */
using GHMMSignalDurationStatePtr
  = std::shared_ptr<GHMMSignalDurationState>;

/**
 * @class GHMMSignalDurationState
 * @brief TODO
 */
class GHMMSignalDurationState
    : public GHMMStateCrtp<GHMMSignalDurationState> {
 public:
  // Alias
  using Base = GHMMStateCrtp<GHMMSignalDurationState>;

  using Self = GHMMSignalDurationState;
  using SelfPtr = std::shared_ptr<Self>;

  // Constructors
  GHMMSignalDurationState(Id id,
                          ProbabilisticModelPtr observation,
                          DiscreteIIDModelPtr transition,
                          unsigned int duration_size);

  // Overriden methods
  Probability durationProbability(unsigned int length) const override;
  unsigned int maximumDurationSize() const override;
  RangePtr durations() const override;

 private:
  // Instance variables
  unsigned int _duration_size;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_GHMM_SIGNAL_DURATION_STATE_
