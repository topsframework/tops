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

#ifndef TOPS_MODEL_SIGNAL_DURATION_
#define TOPS_MODEL_SIGNAL_DURATION_

// Standard headers
#include <memory>

// ToPS templates
#include "model/DurationCrtp.tcc"

namespace tops {
namespace model {

// Forward declaration
class SignalDuration;

/**
 * @typedef SignalDurationPtr
 * @brief Alias of pointer to SignalDuration.
 */
using SignalDurationPtr = std::shared_ptr<SignalDuration>;

/**
 * @class SignalDuration
 * @brief TODO
 */
class SignalDuration : public DurationCrtp<SignalDuration> {
 public:
  // Alias
  using Self = SignalDuration;
  using SelfPtr = std::shared_ptr<Self>;
  using Base = DurationCrtp<Self>;

  // Constructors
  explicit SignalDuration(unsigned int duration_size);

  // Overriden methods
  RangePtr range() const override;
  unsigned int maximumSize() const override;
  Probability probabilityOfLenght(unsigned int length) const override;

 private:
  // Instance variables
  unsigned int _duration_size;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_SIGNAL_DURATION_
