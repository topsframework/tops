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
  /*========================[ PURELY VIRTUAL METHODS ]========================*/

  /**
   * Factory of Simple/Cached Evaluators.
   * @param sequence Sequence to be evaluated
   * @param cached Type of Evaluator (Simple or Cached)
   * @return New instance of EvaluatorPtr<Standard>
   */
  virtual EvaluatorPtr<Standard> standardEvaluator(
      const Standard<Sequence>& sequence, bool cached = false) = 0;

  /**
   * Factory of Simple Generators.
   * @param rng Random Number Generator
   * @return New instance of EvaluatorPtr<Standard>
   */
  virtual GeneratorPtr<Standard> standardGenerator(
      RandomNumberGeneratorPtr rng = RNGAdapter<std::mt19937>::make()) = 0;

  /**
   * Factory of Simple Serializers.
   * @param translator Visitor that serializes the files
   * @return New instance of SerializerPtr
   */
  virtual SerializerPtr serializer(TranslatorPtr translator) = 0;

  /*==============================[ DESTRUCTOR ]==============================*/

  virtual ~ProbabilisticModel() = default;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PROBABILISTIC_MODEL_
