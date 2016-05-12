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

#ifndef TOPS_MODEL_INHOMOGENEOUS_MARKOV_CHAIN_
#define TOPS_MODEL_INHOMOGENEOUS_MARKOV_CHAIN_

// Standard headers
#include <memory>
#include <vector>

// Internal headers
#include "model/ProbabilisticModel.hpp"
#include "model/VariableLengthMarkovChain.hpp"

namespace tops {
namespace model {

// Forward declaration
class InhomogeneousMarkovChain;

/**
 * @typedef InhomogeneousMarkovChainPtr
 * @brief Alias of pointer to InhomogeneousMarkovChain.
 */
using InhomogeneousMarkovChainPtr = std::shared_ptr<InhomogeneousMarkovChain>;

/**
 * @class InhomogeneousMarkovChain
 * @brief Class that represents an inhomogeneous Markov chain.
 *
 * An inhomogeneous Markov chain is a model which suports different Markov
 * chains per position.
 */
class InhomogeneousMarkovChain
    : public ProbabilisticModelCrtp<InhomogeneousMarkovChain> {
 public:
  // Alias
  using Base = ProbabilisticModelCrtp<InhomogeneousMarkovChain>;

  // Constructors
  explicit InhomogeneousMarkovChain(
      std::vector<VariableLengthMarkovChainPtr> vlmcs);

  // Overriden methods
  Probability evaluateSymbol(SEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;

  Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                              unsigned int pos,
                              unsigned int phase,
                              const Sequence& context) const override;

  // Virtual methods
  virtual unsigned int maximumTimeValue();

 protected:
  // Instance variables
  std::vector<VariableLengthMarkovChainPtr> _vlmcs;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_INHOMOGENEOUS_MARKOV_CHAIN_
