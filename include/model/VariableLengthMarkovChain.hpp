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
  static SelfPtr train(TrainerPtr<Standard, Self> trainer,
                       context_algorithm,
                       unsigned int alphabet_size,
                       double delta);

  static SelfPtr train(TrainerPtr<Standard, Self> trainer,
                       fixed_length_algorithm,
                       unsigned int order,
                       unsigned int alphabet_size,
                       double pseudo_counts,
                       std::vector<double> weights,
                       ProbabilisticModelPtr apriori);

  static SelfPtr train(TrainerPtr<Standard, Self> trainer,
                       interpolation_algorithm,
                       std::vector<double> weights,
                       unsigned int alphabet_size,
                       unsigned int order,
                       double pseudo_counts,
                       ProbabilisticModelPtr apriori);

  /*==========================[ OVERRIDEN METHODS ]===========================*/
  /*-------------------------( Probabilistic Model )--------------------------*/

  // StandardEvaluator
  Probability evaluateSymbol(SEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(SEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  // CachedEvaluator
  void initializeCache(CEPtr<Standard> evaluator,
                       unsigned int phase) override;
  Probability evaluateSymbol(CEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(CEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  // StandardGenerator
  Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                              unsigned int pos,
                              unsigned int phase,
                              const Sequence& context) const override;
  Standard<Sequence> drawSequence(SGPtr<Standard> generator,
                                  unsigned int size,
                                  unsigned int phase) const override;

  // SimpleSerializer
  void serialize(SSPtr serializer) override;

 private:
  // Instance variables
  ContextTreePtr _context_tree;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_VARIABLE_LENGTH_MARKOV_CHAIN_
