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

#ifndef TOPS_MODEL_MAXIMAL_DEPENDENCE_DECOMPOSITION_
#define TOPS_MODEL_MAXIMAL_DEPENDENCE_DECOMPOSITION_

// Standard headers
#include <memory>
#include <string>
#include <vector>

// Internal headers
#include "model/Consensus.hpp"
#include "model/InhomogeneousMarkovChain.hpp"
#include "model/MaximalDependenceDecompositionNode.hpp"

namespace tops {
namespace model {

// Forward declaration
class MaximalDependenceDecomposition;

/**
 * @typedef MaximalDependenceDecompositionPtr
 * @brief Alias of pointer to MaximalDependenceDecomposition.
 */
using MaximalDependenceDecompositionPtr
  = std::shared_ptr<MaximalDependenceDecomposition>;

/**
 * @class MaximalDependenceDecomposition
 * @brief TODO
 */
class MaximalDependenceDecomposition
    : public ProbabilisticModelCrtp<MaximalDependenceDecomposition> {
 public:
  // Tags
  class standard_training_algorithm {};

  // Aliases
  using Base = ProbabilisticModelCrtp<MaximalDependenceDecomposition>;
  using Self = MaximalDependenceDecomposition;
  using SelfPtr = MaximalDependenceDecompositionPtr;

  /*=============================[ CONSTRUCTORS ]=============================*/

  MaximalDependenceDecomposition(ConsensusSequence consensus_sequence,
                                 ProbabilisticModelPtr consensus_model,
                                 MaximalDependenceDecompositionNodePtr tree);

  /*============================[ STATIC METHODS ]============================*/

  // Trainer
  static SelfPtr train(TrainerPtr<Standard, Self> trainer,
                       standard_training_algorithm,
                       size_t alphabet_size,
                       ConsensusSequence consensus_sequence,
                       ProbabilisticModelPtr consensus_model,
                       size_t minimum_subset);

  /*==========================[ OVERRIDEN METHODS ]===========================*/
  /*-------------------------( Probabilistic Model )--------------------------*/

  // SimpleEvaluator
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

  // SimpleGenerator
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
  MaximalDependenceDecompositionNodePtr _mdd_tree;
  ConsensusSequence _consensus_sequence;
  ProbabilisticModelPtr _consensus_model;
  std::vector<Probability> _prefix_sum_array;

  /*============================[ STATIC METHODS ]============================*/

  static MaximalDependenceDecompositionNodePtr trainTree(
      std::vector<Sequence> training_set,
      int divmin,
      size_t alphabet_size,
      ConsensusSequence consensus_sequence,
      ProbabilisticModelPtr consensus_model);

  static MaximalDependenceDecompositionNodePtr newNode(
      std::string node_name,
      std::vector<Sequence>& sequences,
      size_t divmin,
      Sequence selected,
      size_t alphabet_size,
      ConsensusSequence consensus_sequence,
      ProbabilisticModelPtr consensus_model);

  static InhomogeneousMarkovChainPtr trainInhomogeneousMarkovChain(
      std::vector<Sequence>& sequences,
      size_t alphabet_size);

  static int getMaximalDependenceIndex(
      InhomogeneousMarkovChainPtr model,
      Sequence selected,
      ConsensusSequence consensus_sequence,
      size_t alphabet_size,
      ProbabilisticModelPtr consensus_model);

  static void subset(int index,
                     std::vector<Sequence>& sequences,
                     std::vector<Sequence>& consensus,
                     std::vector<Sequence>& nonconsensus,
                     ConsensusSequence consensus_sequence);

  /*==========================[ CONCRETE METHODS ]============================*/

  Probability _probabilityOf(const Sequence& s,
                             MaximalDependenceDecompositionNodePtr node,
                             std::vector<int>& indexes) const;

  void _drawAux(Sequence& s, MaximalDependenceDecompositionNodePtr node) const;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_MAXIMAL_DEPENDENCE_DECOMPOSITION_
