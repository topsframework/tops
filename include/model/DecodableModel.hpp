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

// Internal headers
#include "model/Labeler.hpp"
#include "model/Labeling.hpp"
#include "model/Calculator.hpp"
#include "model/ProbabilisticModel.hpp"

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
  /*========================[ PURELY VIRTUAL METHODS ]========================*/

  /**
   * Factory of Simple/Cached Evaluators for labeled sequences.
   * @param sequence Labeled sequence to be evaluated
   * @param cached Type of Evaluator (Simple or Cached)
   * @return New instance of EvaluatorPtr<Labeling>
   */
  virtual EvaluatorPtr<Labeling> labelingEvaluator(
      const Labeling<Sequence>& labeling,
      bool cached = false) = 0;

  /**
   * Factory of Simple Generators for labeled sequences.
   * @param rng Random Number Generator
   * @return New instance of EvaluatorPtr<Labeling>
   */
  virtual GeneratorPtr<Labeling> labelingGenerator(
      RandomNumberGeneratorPtr rng = RNGAdapter<std::mt19937>::make()) = 0;

  /**
   * Factory of Simple/Cached Labelers
   * @param sequences Input sequence to be labeled
   * @param cached Type of Labeler (cached or non-cached)
   * @return New instance of LabelerPtr
   */
  virtual LabelerPtr labeler(const Multiple<Sequence>& sequence,
                             bool cached = false) = 0;

  /**
   * Factory of Simple/Cached Calculators
   * @param sequence Input sequence that will be used for calculations
   * @param other_sequences Features associated with the input sequence
   * @param cached Type of Calculator (cached or non-cached)
   * @return New instance of CalculatorPtr
   */
  virtual CalculatorPtr calculator(const Multiple<Sequence>& sequence,
                                   bool cached = false) = 0;

  /*==============================[ DESTRUCTOR ]==============================*/

  virtual ~DecodableModel() = default;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_DECODABLE_MODEL_
