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

#ifndef TOPS_MODEL_FIXED_DURATION_
#define TOPS_MODEL_FIXED_DURATION_

// Standard headers
#include <memory>

// Internal headers
#include "model/DurationCrtp.hpp"

namespace tops {
namespace model {

// Forward declaration
class FixedDuration;

/**
 * @typedef FixedDurationPtr
 * @brief Alias of pointer to FixedDuration.
 */
using FixedDurationPtr = std::shared_ptr<FixedDuration>;

/**
 * @class FixedDuration
 * @brief TODO
 */
class FixedDuration : public DurationCrtp<FixedDuration> {
 public:
  // Alias
  using Self = FixedDuration;
  using SelfPtr = std::shared_ptr<Self>;
  using Base = DurationCrtp<Self>;

  // Constructors
  explicit FixedDuration(unsigned int duration_size);

  // Overriden methods
  Range range() const override;
  unsigned int maximumSize() const override;
  Probability probabilityOfLenght(unsigned int length) const override;

 private:
  // Instance variables
  unsigned int _duration_size;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_FIXED_DURATION_
