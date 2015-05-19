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

// ToPS headers
#include "model/Sequence.hpp"
#include "model/Evaluator.hpp"
#include "model/Generator.hpp"

// ToPS templates
#include "model/SimpleEvaluatorImpl.tcc"
#include "model/CachedEvaluatorImpl.tcc"

namespace tops {
namespace model {

class InhomogeneousMarkovChain;
using InhomogeneousMarkovChainPtr = std::shared_ptr<InhomogeneousMarkovChain>;

/**
 * @class ProbabilisticModel
 * @brief Abstract class that represents all probabilistic models.
 */
class ProbabilisticModel
    : public std::enable_shared_from_this<ProbabilisticModel>{
 public:
  // Alias
  using Cache = std::vector<double>;
  using SEPtr = SimpleEvaluatorImplPtr<ProbabilisticModel>;
  using CEPtr = CachedEvaluatorImplPtr<ProbabilisticModel>;

  // Purely virtual methods
  virtual double evaluate(const Sequence &s,
                          unsigned int pos,
                          unsigned int phase = 0) const = 0;
  virtual Symbol choose(const Sequence &context,
                        unsigned int pos,
                        unsigned int phase = 0) const = 0;

  // Virtual methods
  virtual Sequence choose(unsigned int size,
                          unsigned int phase = 0) const;

  virtual GeneratorPtr generator();

  virtual EvaluatorPtr evaluator(const Sequence &s, bool cached = false);

  virtual InhomogeneousMarkovChain* inhomogeneous();

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
};

/**
 * @typedef ProbabilisticModelPtr
 * @brief Alias of pointer to ProbabilisticModel.
 */
using ProbabilisticModelPtr = std::shared_ptr<ProbabilisticModel>;

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PROBABILISTIC_MODEL_
