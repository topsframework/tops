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

#ifndef TOPS_MODEL_GEOMETRIC_DURATION_
#define TOPS_MODEL_GEOMETRIC_DURATION_

// Standard headers
#include <cmath>
#include <memory>

// ToPS headers
#include "model/ProbabilisticModel.hpp"

// ToPS templates
#include "model/DurationCrtp.tcc"

namespace tops {
namespace model {

// Forward declaration
class GeometricDuration;

/**
 * @typedef GeometricDurationPtr
 * @brief Alias of pointer to GeometricDuration.
 */
using GeometricDurationPtr = std::shared_ptr<GeometricDuration>;

/**
 * @class GeometricDuration
 * @brief TODO
 */
class GeometricDuration : public DurationCrtp<GeometricDuration> {
 public:
  // Alias
  using Self = GeometricDuration;
  using SelfPtr = std::shared_ptr<Self>;
  using Base = DurationCrtp<Self>;

  // Constructors
  GeometricDuration(unsigned int id,
                    ProbabilisticModelPtr transition);

  // Overriden methods
  RangePtr range() const override;
  unsigned int maximumSize() const override;
  Probability durationProbability(unsigned int length) const override;

 private:
  // Instance variables
  unsigned int _id;
  ProbabilisticModelPtr _transition;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL__GEOMETRIC_DURATION_
