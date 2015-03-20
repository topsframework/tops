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

#ifndef TOPS_MODEL_TARGET_MODEL_
#define TOPS_MODEL_TARGET_MODEL_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "model/DiscreteIIDModel.hpp"

namespace tops {
namespace model {

class TargetModel;

/**
 * @typedef TargetModelPtr
 * @brief Alias of pointer to TargetModel.
 */
using TargetModelPtr = std::shared_ptr<TargetModel>;

/**
 * @class TargetModel
 * @brief TODO
 */
class TargetModel : public DiscreteIIDModel {
 public:
  // Static methods
  static TargetModelPtr make(int alphabet_size);

  // Virtual methods
  virtual double evaluatePosition(const Sequence &s, unsigned int i, unsigned int phase = 0) const;

  // Concrete methods
  DiscreteIIDModelPtr sequenceDistribution(const Sequence &s) const;

 private:
  // Constructors
  TargetModel(int alphabet_size);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_TARGET_MODEL_
