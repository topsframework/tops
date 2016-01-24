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

#ifndef TOPS_MODEL_DURATION_
#define TOPS_MODEL_DURATION_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Range.hpp"
#include "model/Serializer.hpp"
#include "model/Probability.hpp"

namespace tops {
namespace model {

// Forward declaration
class Duration;

/**
 * @typedef DurationPtr
 * @brief Alias of pointer to Duration.
 */
using DurationPtr = std::shared_ptr<Duration>;

/**
 * @class Duration
 * @brief TODO
 */
class Duration {
 public:
  // Purely virtual methods
  virtual SerializerPtr serializer(TranslatorPtr translator) = 0;

  virtual RangePtr range() const = 0;
  virtual unsigned int maximumSize() const = 0;
  virtual Probability probabilityOfLenght(unsigned int length) const = 0;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_DURATION_
