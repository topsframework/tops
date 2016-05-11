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

// Interface header
#include "helper/MaximalDependenceDecomposition.hpp"

// Standard headers
#include <cmath>
#include <vector>

// ToPS headers
#include "helper/DiscreteIIDModel.hpp"

namespace tops {
namespace helper {

/*----------------------------------------------------------------------------*/
/*                                 FUNCTIONS                                  */
/*----------------------------------------------------------------------------*/

tops::model::MaximalDependenceDecompositionPtr createSampleMDD() {
  model::ConsensusSequence consensus_sequence = createConsensusSequence();

  model::ProbabilisticModelPtr consensus_model = createDNAIIDModel();

  auto mdd_root = model::MaximalDependenceDecompositionNode::make(
      "root", createDNAIIDModel(), 7);
  auto mdd_g5 = model::MaximalDependenceDecompositionNode::make(
      "g5", createDNAIIDModel(), 2);
  auto mdd_h5 = model::MaximalDependenceDecompositionNode::make(
      "h5", createDNAIIDModel(), -1);
  auto mdd_g5gm1 = model::MaximalDependenceDecompositionNode::make(
      "g5gm1", createDNAIIDModel(), 1);
  auto mdd_g5hm1 = model::MaximalDependenceDecompositionNode::make(
      "g5hm1", createDNAIIDModel(), -1);
  auto mdd_g5gm1am2 = model::MaximalDependenceDecompositionNode::make(
      "g5gm1am2", createDNAIIDModel(), 8);
  auto mdd_g5gm1bm2 = model::MaximalDependenceDecompositionNode::make(
      "g5gm1bm2", createDNAIIDModel(), -1);
  auto mdd_g5gm1am2u6 = model::MaximalDependenceDecompositionNode::make(
      "g5gm1am2u6", createDNAIIDModel(), -1);
  auto mdd_g5gm1am2v6 = model::MaximalDependenceDecompositionNode::make(
      "g5gm1am2v6", createDNAIIDModel(), -1);

  mdd_root->setChildern(mdd_g5, mdd_h5);
  mdd_g5->setChildern(mdd_g5gm1, mdd_g5hm1);
  mdd_g5gm1->setChildern(mdd_g5gm1am2, mdd_g5gm1bm2);
  mdd_g5gm1am2->setChildern(mdd_g5gm1am2u6, mdd_g5gm1am2v6);

  return model::MaximalDependenceDecomposition::make(
      consensus_sequence, consensus_model, mdd_root);
}

/*----------------------------------------------------------------------------*/

tops::model::ConsensusSequence createConsensusSequence() {
  tops::model::ConsensusSequence consensus_sequence;

  std::vector<tops::model::Symbol> s1;
  s1.push_back(0); s1.push_back(1);  // a/c
  tops::model::Consensus c1(s1);
  consensus_sequence.push_back(c1);

  int x1[] = {0, 2, 2, 3};  // AGGT
  for (int i = 0; i < 4; i++) {
    std::vector<tops::model::Symbol> s;
    s.push_back(x1[i]);
    tops::model::Consensus c(s);
    consensus_sequence.push_back(c);
  }

  std::vector<tops::model::Symbol> s2;
  s2.push_back(0); s2.push_back(2);  // a/g
  tops::model::Consensus c2(s2);
  consensus_sequence.push_back(c2);

  int x2[] = {0, 2, 3};  // agt
  for (int i = 0; i < 3; i++) {
    std::vector<tops::model::Symbol> s;
    s.push_back(x2[i]);
    tops::model::Consensus c(s);
    consensus_sequence.push_back(c);
  }
  return consensus_sequence;
}

/*----------------------------------------------------------------------------*/

}  // namespace helper
}  // namespace tops
