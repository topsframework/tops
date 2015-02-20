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
#include <vector>
#include <string>

// ToPS headers
#include "FactorableModel.hpp"
#include "MaximalDependenceDecompositionNode.hpp"
#include "Consensus.hpp"
#include "InhomogeneousMarkovChain.hpp"

namespace tops {
namespace model {

class MaximalDependenceDecomposition;

/**
 * @typedef MaximalDependenceDecompositionPtr
 * @brief Alias of pointer to MaximalDependenceDecomposition.
 */
using MaximalDependenceDecompositionPtr = std::shared_ptr<MaximalDependenceDecomposition>;

/**
 * @class MaximalDependenceDecomposition
 * @brief TODO
 */
class MaximalDependenceDecomposition : public FactorableModel {
 public:
  static MaximalDependenceDecompositionPtr make(
      unsigned int alphabet_size,
      ConsensusSequence consensus_sequence,
      ProbabilisticModelPtr consensus_model,
      MaximalDependenceDecompositionNodePtr tree);
  static MaximalDependenceDecompositionPtr train(
      std::vector<Sequence> training_set,
      unsigned int alphabet_size,
      ConsensusSequence consensus_sequence,
      ProbabilisticModelPtr consensus_model,
      unsigned int minimum_subset);

  virtual int alphabetSize() const;
  virtual double evaluateSequence(const Sequence &s,
                                  unsigned int begin,
                                  unsigned int end) const;
  virtual double evaluatePosition(const Sequence &s, unsigned int i) const;
  virtual Sequence chooseSequence(Sequence &s, unsigned int size) const;
  virtual Symbol choosePosition(const Sequence &s, unsigned int i) const;

 private:
  MaximalDependenceDecomposition(unsigned int alphabet_size, ConsensusSequence consensus_sequence, ProbabilisticModelPtr consensus_model, MaximalDependenceDecompositionNodePtr tree);
  double _evaluateAux(const Sequence & s, MaximalDependenceDecompositionNodePtr node, std::vector<int> &indexes) const;
  void _chooseAux(Sequence & s, MaximalDependenceDecompositionNodePtr node) const;
  static MaximalDependenceDecompositionNodePtr trainTree(
      std::vector<Sequence> training_set,
      int divmin,
      unsigned int alphabet_size,
      ConsensusSequence consensus_sequence,
      ProbabilisticModelPtr consensus_model);
  static MaximalDependenceDecompositionNodePtr newNode(
      std::string node_name,
      std::vector<Sequence> & sequences,
      int divmin,
      Sequence selected,
      unsigned int alphabet_size,
      ConsensusSequence consensus_sequence,
      ProbabilisticModelPtr consensus_model);
  static InhomogeneousMarkovChainPtr trainInhomogeneousMarkovChain(
      std::vector<Sequence> & sequences,
      unsigned int alphabet_size);
  static int getMaximalDependenceIndex(
      InhomogeneousMarkovChainPtr model,
      Sequence selected,
      ConsensusSequence consensus_sequence,
      unsigned int alphabet_size,
      ProbabilisticModelPtr consensus_model);
  static void subset(int index,
                     std::vector<Sequence> & sequences,
                     std::vector<Sequence> & consensus,
                     std::vector<Sequence> & nonconsensus,
                     ConsensusSequence consensus_sequence);


  MaximalDependenceDecompositionNodePtr _mdd_tree;
  ConsensusSequence _consensus_sequence;
  ProbabilisticModelPtr _consensus_model;
  unsigned int _alphabet_size;
  std::vector<double> _prefix_sum_array;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_MAXIMAL_DEPENDENCE_DECOMPOSITION_
