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

#ifndef TOPS_MODEL_EXPLICIT_DURATION_
#define TOPS_MODEL_EXPLICIT_DURATION_

// Standard headers
#include <memory>

// ToPS headers
#include "model/ProbabilisticModel.hpp"

// ToPS templates
#include "model/DurationCrtp.tcc"

namespace tops {
namespace model {

// Forward declaration
class ExplicitDuration;

/**
 * @typedef ExplicitDurationPtr
 * @brief Alias of pointer to ExplicitDuration.
 */
using ExplicitDurationPtr = std::shared_ptr<ExplicitDuration>;

/**
 * @class ExplicitDuration
 * @brief TODO
 */
class ExplicitDuration : public DurationCrtp<ExplicitDuration> {
 public:
  // Alias
  using Self = ExplicitDuration;
  using SelfPtr = std::shared_ptr<Self>;
  using Base = DurationCrtp<Self>;

  // Constructors
  ExplicitDuration(ProbabilisticModelPtr duration,
                   unsigned int max_duration = 100);

  // Overriden methods
  RangePtr range() const override;
  unsigned int maximumDurationSize() const override;
  Probability durationProbability(unsigned int length) const override;

 private:
  // Instance variables
  ProbabilisticModelPtr _duration;
  unsigned int _max_duration_size;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_EXPLICIT_DURATION_
