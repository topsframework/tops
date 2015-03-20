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

// ToPS templates
#include "model/SimpleEvaluator.tcc"
#include "model/CachedEvaluator.tcc"

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
  using cache = std::vector<double>;
  using SEPtr = SimpleEvaluatorPtr<ProbabilisticModel>;
  using CEPtr = CachedEvaluatorPtr<ProbabilisticModel>;

  // Purely virtual methods
  virtual double evaluatePosition(const Sequence &s, unsigned int i,
                                  unsigned int phase = 0) const = 0;
  virtual Symbol choosePosition(const Sequence &s, unsigned int i,
                                unsigned int phase = 0) const = 0;

  // Virtual methods
  virtual Sequence chooseSequence(Sequence &s, unsigned int size,
                                  unsigned int phase = 0) const;

  virtual EvaluatorPtr evaluate(const Sequence &s, bool cached = false);

  virtual InhomogeneousMarkovChain* inhomogeneous();

  // Concrete methods
  double probabilityOf(SEPtr evaluator,
                       unsigned int begin,
                       unsigned int end,
                       unsigned int phase = 0) const;

  void initializeCachedEvaluator(CEPtr evaluator,
                                unsigned int phase = 0);
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
