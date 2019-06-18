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

// Internal headers
#include "model/ContextTree.hpp"
#include "model/ProbabilisticModel.hpp"

namespace tops {
namespace model {

// Forward declaration
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
  // Tags
  class context_algorithm {};
  class fixed_length_algorithm {};
  class interpolation_algorithm {};

  // Aliases
  using Self = VariableLengthMarkovChain;
  using SelfPtr = VariableLengthMarkovChainPtr;
  using Base = ProbabilisticModelCrtp<Self>;

  /*=============================[ CONSTRUCTORS ]=============================*/

  explicit VariableLengthMarkovChain(ContextTreePtr context_tree);

  /*============================[ STATIC METHODS ]============================*/

  // Trainer
  static SelfPtr train(TrainerPtr<Multiple, Self> trainer,
                       context_algorithm,
                       size_t alphabet_size,
                       double delta);

  static SelfPtr train(TrainerPtr<Multiple, Self> trainer,
                       fixed_length_algorithm,
                       size_t order,
                       size_t alphabet_size,
                       double pseudo_counts,
                       std::vector<double> weights,
                       ProbabilisticModelPtr apriori);

  static SelfPtr train(TrainerPtr<Multiple, Self> trainer,
                       interpolation_algorithm,
                       std::vector<double> weights,
                       size_t alphabet_size,
                       size_t order,
                       double pseudo_counts,
                       ProbabilisticModelPtr apriori);

  /*==========================[ OVERRIDEN METHODS ]===========================*/
  /*-------------------------( Probabilistic Model )--------------------------*/

  // StandardEvaluator
  Probability evaluateSymbol(SEPtr<Multiple> evaluator,
                             size_t pos,
                             size_t phase) const override;
  Probability evaluateSequence(SEPtr<Multiple> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const override;

  // CachedEvaluator
  void initializeCache(CEPtr<Multiple> evaluator,
                       size_t phase) override;
  Probability evaluateSymbol(CEPtr<Multiple> evaluator,
                             size_t pos,
                             size_t phase) const override;
  Probability evaluateSequence(CEPtr<Multiple> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const override;

  // StandardGenerator
  Multiple<Symbol> drawSymbol(SGPtr<Multiple> generator,
                              size_t pos,
                              size_t phase,
                              const Multiple<Sequence>& context) const override;
  Multiple<Sequence> drawSequence(SGPtr<Multiple> generator,
                                  size_t size,
                                  size_t phase) const override;

  // SimpleSerializer
  void serialize(SSPtr serializer) override;

 private:
  // Instance variables
  ContextTreePtr _context_tree;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_VARIABLE_LENGTH_MARKOV_CHAIN_
