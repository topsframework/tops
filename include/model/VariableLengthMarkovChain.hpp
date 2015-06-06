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

#ifndef TOPS_MODEL_VARIABLE_LENGTH_MARKOV_CHAIN_
#define TOPS_MODEL_VARIABLE_LENGTH_MARKOV_CHAIN_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "model/ContextTree.hpp"
#include "model/ProbabilisticModel.hpp"

namespace tops {
namespace model {

class VariableLengthMarkovChain;

/**
 * @typedef VariableLengthMarkovChainPtr
 * @brief Alias of pointer to VariableLengthMarkovChain.
 */
using VariableLengthMarkovChainPtr
    = std::shared_ptr<VariableLengthMarkovChain>;

/**
 * @class VariableLengthMarkovChain
 * @brief Class that represents an variable length Markov chain.
 *
 * An variable length Markov chain is a Markov chain that can have different
 * size contexts.
 */
class VariableLengthMarkovChain
    : public ProbabilisticModelCrtp<VariableLengthMarkovChain> {
 public:
  // Alias
  using Base = ProbabilisticModelCrtp<VariableLengthMarkovChain>;

  // Static methods
  static VariableLengthMarkovChainPtr make(ContextTreePtr context_tree);
  static VariableLengthMarkovChainPtr trainContextAlgorithm(
      std::vector<Sequence> training_set,
      unsigned int alphabet_size,
      double delta);
  static VariableLengthMarkovChainPtr trainFixedLengthMarkovChain(
      std::vector<Sequence> training_set,
      unsigned int order,
      unsigned int alphabet_size,
      double pseudo_counts,
      std::vector<double> weights,
      ProbabilisticModelPtr apriori);
  static VariableLengthMarkovChainPtr trainInterpolatedMarkovChain(
      std::vector<Sequence> training_set,
      std::vector<double> weights,
      unsigned int alphabet_size,
      unsigned int order,
      double pseudo_counts,
      ProbabilisticModelPtr apriori);

  // Virtual methods
  virtual double evaluate(const Sequence &s,
                          unsigned int pos,
                          unsigned int phase = 0) const override;
  virtual Symbol choose(const Sequence &context,
                        unsigned int pos,
                        unsigned int phase = 0) const override;

 private:
  // Instance variables
  ContextTreePtr _context_tree;

  // Constructors
  explicit VariableLengthMarkovChain(ContextTreePtr context_tree);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_VARIABLE_LENGTH_MARKOV_CHAIN_
