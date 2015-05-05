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

#ifndef TOPS_MODEL_PHASED_INHOMOGENEOUS_MARKOV_CHAIN_
#define TOPS_MODEL_PHASED_INHOMOGENEOUS_MARKOV_CHAIN_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "model/InhomogeneousMarkovChain.hpp"
#include "model/VariableLengthMarkovChain.hpp"
#include "model/Matrix.hpp"

namespace tops {
namespace model {

class PhasedInhomogeneousMarkovChain;

/**
 * @typedef PhasedInhomogeneousMarkovChainPtr
 * @brief Alias of pointer to PhasedInhomogeneousMarkovChain.
 */
using PhasedInhomogeneousMarkovChainPtr
    = std::shared_ptr<PhasedInhomogeneousMarkovChain>;

/**
 * @class PhasedInhomogeneousMarkovChain
 * @brief Class that represents an inhomogeneous Markov chain.
 *
 * A phased inhomogeneous Markov chain is a model which suports different
 * Markov chains per position. Each Markov chain repeats itsealf per
 * phase.
 */
class PhasedInhomogeneousMarkovChain : public InhomogeneousMarkovChain {
 public:
  // Alias
  using Cache = Matrix;
  using SEPtr = SimpleEvaluatorImplPtr<PhasedInhomogeneousMarkovChain>;
  using CEPtr = CachedEvaluatorImplPtr<PhasedInhomogeneousMarkovChain>;

  // Static methods
  static PhasedInhomogeneousMarkovChainPtr make(
      std::vector<VariableLengthMarkovChainPtr> vlmcs);
  static PhasedInhomogeneousMarkovChainPtr trainInterpolatedPhasedMarkovChain(
      std::vector<Sequence> training_set,
      unsigned int alphabet_size,
      unsigned int order,
      unsigned int nphases,
      double pseudo_counts,
      std::vector<double> weights,
      ProbabilisticModelPtr apriori);

  // Virtual methods

  virtual double evaluate(const Sequence &s, unsigned int pos, unsigned int phase = 0) const;
  virtual Symbol choosePosition(const Sequence &s,
                                unsigned int pos,
                                unsigned int phase = 0) const override;

  virtual EvaluatorPtr evaluator(const Sequence &s, bool cached = false);

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
  // Constructors
  PhasedInhomogeneousMarkovChain(
      std::vector<VariableLengthMarkovChainPtr> vlmcs);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PHASED_INHOMOGENEOUS_MARKOV_CHAIN_
