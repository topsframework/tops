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

#ifndef TOPS_MODEL_LABELER_
#define TOPS_MODEL_LABELER_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Labeling.hpp"
#include "model/Sequence.hpp"
#include "model/Estimation.hpp"

namespace tops {
namespace model {

// Forward declaration
class Labeler;

/**
 * @typedef LabelerPtr
 * @brief Alias of pointer to Labeler.
 */
using LabelerPtr = std::shared_ptr<Labeler>;

/**
 * @class Labeler
 * @brief TODO
 */
class Labeler : public std::enable_shared_from_this<Labeler> {
 public:
  // Enum classes
  enum class method { bestPath, posteriorDecoding };

  // Purely virtual methods
  virtual Estimation<Labeling<Sequence>>
  labeling(const method& method) const = 0;

  virtual Sequence& sequence() = 0;
  virtual const Sequence& sequence() const = 0;

  // Destructor
  virtual ~Labeler() = default;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_LABELER_
