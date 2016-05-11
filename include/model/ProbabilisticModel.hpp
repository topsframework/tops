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

#ifndef TOPS_MODEL_PROBABILISTIC_MODEL_
#define TOPS_MODEL_PROBABILISTIC_MODEL_

// Standard headers
#include <memory>
#include <random>

// Internal headers
#include "model/Sequence.hpp"
#include "model/Standard.hpp"
#include "model/Evaluator.hpp"
#include "model/Generator.hpp"
#include "model/Serializer.hpp"
#include "model/RandomNumberGenerator.hpp"
#include "model/RandomNumberGeneratorAdapter.hpp"

namespace tops {
namespace model {

// Forward declaration
class ProbabilisticModel;

/**
 * @typedef ProbabilisticModelPtr
 * @brief Alias of pointer to ProbabilisticModel.
 */
using ProbabilisticModelPtr = std::shared_ptr<ProbabilisticModel>;

/**
 * @class ProbabilisticModel
 * @brief Abstract class that represents all probabilistic models.
 */
class ProbabilisticModel {
 public:
  // Purely virtual methods
  virtual EvaluatorPtr<Standard> standardEvaluator(
      const Standard<Sequence>& sequence, bool cached = false) = 0;

  virtual GeneratorPtr<Standard> standardGenerator(
      RandomNumberGeneratorPtr rng = RNGAdapter<std::mt19937>::make()) = 0;

  virtual SerializerPtr serializer(TranslatorPtr translator) = 0;

  // Destructor
  virtual ~ProbabilisticModel() = default;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PROBABILISTIC_MODEL_
