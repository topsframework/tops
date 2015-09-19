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

#ifndef TOPS_MODEL_DECODABLE_MODEL_
#define TOPS_MODEL_DECODABLE_MODEL_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "model/ProbabilisticModel.hpp"

// ToPS templates
#include "model/Labeling.tcc"

namespace tops {
namespace model {

// Forward declaration
class DecodableModel;

/**
 * @typedef DecodableModelPtr
 * @brief Alias of pointer to DecodableModel.
 */
using DecodableModelPtr = std::shared_ptr<DecodableModel>;

/**
 * @class DecodableModel
 * @brief TODO
 */
class DecodableModel : public virtual ProbabilisticModel {
 public:
  // Purely virtual methods
  virtual EvaluatorPtr<Labeling> labelingEvaluator(
      const Labeling<Sequence> &sequence, bool cached = false) = 0;

  virtual GeneratorPtr<Labeling> labelingGenerator(
      RandomNumberGeneratorPtr rng = RNGAdapter<std::mt19937>::make(42)) = 0;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_DECODABLE_MODEL_
