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

// ToPS headers
#include "model/ProbabilisticModel.hpp"
#include "model/VariableLengthMarkovChain.hpp"

namespace tops {
namespace model {

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
class InhomogeneousMarkovChain : public ProbabilisticModel {
 public:
  // Static methods
  static InhomogeneousMarkovChainPtr make(
      std::vector<VariableLengthMarkovChainPtr> vlmcs);

  // Virtual methods
  virtual double evaluatePosition(const Sequence &s, unsigned int i, unsigned int phase = 0) const;
  virtual Symbol choosePosition(const Sequence &s, unsigned int i, unsigned int phase = 0) const;

 protected:
  // Instance variables
  std::vector<VariableLengthMarkovChainPtr> _vlmcs;

  // Constructors
  InhomogeneousMarkovChain(
      std::vector<VariableLengthMarkovChainPtr> vlmcs);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_INHOMOGENEOUS_MARKOV_CHAIN_
