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

#ifndef TOPS_MODEL_MULTIPLE_SEQUENTIAL_MODEL_
#define TOPS_MODEL_MULTIPLE_SEQUENTIAL_MODEL_

// Standard headers
#include <memory>
#include <vector>

// Internal headers
#include "model/ProbabilisticModel.hpp"
#include "model/ProbabilisticModelCrtp.hpp"

namespace tops {
namespace model {

// Forward declaration
class MultipleSequentialModel;

/**
 * @typedef MultipleSequentialModelPtr
 * @brief Alias of pointer to MultipleSequentialModel.
 */
using MultipleSequentialModelPtr
    = std::shared_ptr<MultipleSequentialModel>;

/**
 * @class MultipleSequentialModel
 * @brief TODO
 */
class MultipleSequentialModel
    : public ProbabilisticModelCrtp<MultipleSequentialModel> {
 public:
  // Aliases
  using Self = MultipleSequentialModel;
  using SelfPtr = MultipleSequentialModelPtr;
  using Base = ProbabilisticModelCrtp<Self>;

  // Inner classes
  struct Cache : Base::Cache {
    std::vector<EvaluatorPtr<Standard>> evaluators;
  };

  /*=============================[ CONSTRUCTORS ]=============================*/

  MultipleSequentialModel(std::vector<ProbabilisticModelPtr> models,
                          std::vector<int> max_length);

  /*==========================[ OVERRIDEN METHODS ]===========================*/
  /*-------------------------( Probabilistic Model )--------------------------*/

  // StandardEvaluator
  Probability evaluateSymbol(SEPtr<Standard> evaluator,
                             size_t pos,
                             size_t phase) const override;
  Probability evaluateSequence(SEPtr<Standard> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const override;

  // CachedEvaluator
  void initializeCache(CEPtr<Standard> evaluator,
                       size_t phase) override;
  Probability evaluateSymbol(CEPtr<Standard> evaluator,
                             size_t pos,
                             size_t phase) const override;
  Probability evaluateSequence(CEPtr<Standard> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const override;

  // StandardGenerator
  Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                              size_t pos,
                              size_t phase,
                              const Sequence& context) const override;
  Standard<Sequence> drawSequence(SGPtr<Standard> generator,
                                  size_t size,
                                  size_t phase) const override;

  // SimpleSerializer
  void serialize(SSPtr serializer) override;

 private:
  // Instance variables
  std::vector<ProbabilisticModelPtr> _models;
  std::vector<int> _max_length;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_MULTIPLE_SEQUENTIAL_MODEL_
