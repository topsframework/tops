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

// ToPS headers
#include "model/ProbabilisticModel.hpp"

namespace tops {
namespace model {

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
class MultipleSequentialModel : public ProbabilisticModel {
 public:
  // Alias
  using Cache = std::vector<EvaluatorPtr>;
  using SEPtr = SimpleEvaluatorImplPtr<MultipleSequentialModel>;
  using CEPtr = CachedEvaluatorImplPtr<MultipleSequentialModel>;

  // Static methods
  static MultipleSequentialModelPtr make(
      std::vector<ProbabilisticModelPtr> models,
      std::vector<int> max_length);

  // Virtual methods
  virtual double evaluate(const Sequence &s,
                          unsigned int pos,
                          unsigned int phase = 0) const override;
  virtual Symbol choose(const Sequence &context,
                        unsigned int pos,
                        unsigned int phase = 0) const override;

  virtual EvaluatorPtr evaluator(const Sequence &s,
                                 bool cached = false) override;

  // Concrete methods
  void initializeCachedEvaluator(CEPtr evaluator,
                                 unsigned int phase = 0);

  double simpleProbabilityOf(SEPtr evaluator,
                             unsigned int begin,
                             unsigned int end,
                             unsigned int phase = 0) const;
  double cachedProbabilityOf(CEPtr evaluator,
                             unsigned int begin,
                             unsigned int end,
                             unsigned int phase = 0) const;

 private:
  // Instance variables
  std::vector<ProbabilisticModelPtr> _models;
  std::vector<int> _max_length;
  unsigned int _idx_not_limited;

  // Constructors
  MultipleSequentialModel(std::vector<ProbabilisticModelPtr> models,
                          std::vector<int> max_length);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_MULTIPLE_SEQUENTIAL_MODEL_
