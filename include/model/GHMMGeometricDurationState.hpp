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

#ifndef TOPS_MODEL_GHMM_GEOMETRIC_DURATION_STATE_
#define TOPS_MODEL_GHMM_GEOMETRIC_DURATION_STATE_

// Standard headers
#include <memory>

// ToPS templates
#include "model/GHMMStateCrtp.tcc"

namespace tops {
namespace model {

class GHMMGeometricDurationState;

/**
 * @typedef GHMMGeometricDurationStatePtr
 * @brief Alias of pointer to GHMMGeometricDurationState.
 */
using GHMMGeometricDurationStatePtr
  = std::shared_ptr<GHMMGeometricDurationState>;

/**
 * @class GHMMGeometricDurationState
 * @brief TODO
 */
class GHMMGeometricDurationState
    : public GHMMStateCrtp<GHMMGeometricDurationState> {
 public:
  // Alias
  using Base = GHMMStateCrtp<GHMMGeometricDurationState>;

  using Self = GHMMGeometricDurationState;
  using SelfPtr = std::shared_ptr<Self>;

  // Constructors
  GHMMGeometricDurationState(Id id,
                             ProbabilisticModelPtr observation,
                             DiscreteIIDModelPtr transition);

  // Overriden methods
  Probability durationProbability(unsigned int length) const override;
  unsigned int maximumDurationSize() const override;
  RangePtr durations() const override;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_GHMM_GEOMETRIC_DURATION_STATE_
