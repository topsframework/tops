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

// Standard headers
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// ToPS headers
#include "MaximalDependenceDecomposition.hpp"
#include "Util.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

MaximalDependenceDecomposition::MaximalDependenceDecomposition(
    ConsensusSequence consensus_sequence,
    ProbabilisticModelPtr consensus_model,
    MaximalDependenceDecompositionNodePtr tree)
      : _mdd_tree(tree),
        _consensus_sequence(consensus_sequence),
        _consensus_model(consensus_model) {
}

/*----------------------------------------------------------------------------*/
/*                              STATIC METHODS                                */
/*----------------------------------------------------------------------------*/

MaximalDependenceDecompositionPtr MaximalDependenceDecomposition::train(
      std::vector<Sequence> training_set,
      unsigned int alphabet_size,
      ConsensusSequence consensus_sequence,
      ProbabilisticModelPtr consensus_model,
      unsigned int minimum_subset) {
  return MaximalDependenceDecomposition::make(
    consensus_sequence,
    consensus_model,
    trainTree(training_set,
              minimum_subset,
              alphabet_size,
              consensus_sequence,
              consensus_model));
}

/*----------------------------------------------------------------------------*/

MaximalDependenceDecompositionNodePtr MaximalDependenceDecomposition::trainTree(
    std::vector<Sequence> training_set,
    int divmin,
    unsigned int alphabet_size,
    ConsensusSequence consensus_sequence,
    ProbabilisticModelPtr consensus_model) {
  Sequence selected;
  return newNode("node_r0",
                 training_set,
                 divmin,
                 selected,
                 alphabet_size,
                 consensus_sequence,
                 consensus_model);
}

/*----------------------------------------------------------------------------*/

MaximalDependenceDecompositionNodePtr MaximalDependenceDecomposition::newNode(
    std::string node_name,
    std::vector<Sequence> & sequences,
    unsigned int divmin,
    Sequence selected,
    unsigned int alphabet_size,
    ConsensusSequence consensus_sequence,
    ProbabilisticModelPtr consensus_model) {
  InhomogeneousMarkovChainPtr model = trainInhomogeneousMarkovChain(
    sequences, alphabet_size);
  MaximalDependenceDecompositionNodePtr mdd_node;

  int consensus_index = getMaximalDependenceIndex(model,
                                                  selected,
                                                  consensus_sequence,
                                                  alphabet_size,
                                                  consensus_model);

  if (consensus_index >= 0) {
    selected.push_back(consensus_index);

    Sequence s(consensus_sequence.size(), INVALID_SYMBOL);
    s[consensus_index] = consensus_sequence[consensus_index].symbols()[0];
    double prob = consensus_model
      ->standardEvaluator(s)->evaluateSymbol(consensus_index);
    if (prob >= -0.001 && prob <= 0.001) {
      mdd_node = MaximalDependenceDecompositionNode::make(node_name,
                                                          model,
                                                          consensus_index);
      std::stringstream p;
      p << node_name << "_p" << consensus_index;
      MaximalDependenceDecompositionNodePtr child = newNode(
        p.str(),
        sequences,
        divmin,
        selected,
        alphabet_size,
        consensus_sequence,
        consensus_model);
      mdd_node->setChild(child);
    } else {
      std::vector<Sequence> consensus_sequences;
      std::vector<Sequence> nonconsensus_sequences;
      subset(consensus_index,
            sequences,
            consensus_sequences,
            nonconsensus_sequences,
            consensus_sequence);

      if ((consensus_sequences.size() > divmin)
          && (nonconsensus_sequences.size() > divmin)) {
        mdd_node = MaximalDependenceDecompositionNode::make(node_name,
                                                            model,
                                                            consensus_index);
        std::stringstream p;
        p << node_name << "_p" << consensus_index;
        MaximalDependenceDecompositionNodePtr left = newNode(
          p.str(),
          consensus_sequences,
          divmin,
          selected,
          alphabet_size,
          consensus_sequence,
          consensus_model);
        std::stringstream n;
        n << node_name << "_n" << consensus_index;
        MaximalDependenceDecompositionNodePtr right = newNode(
          n.str(),
          nonconsensus_sequences,
          divmin,
          selected,
          alphabet_size,
          consensus_sequence,
          consensus_model);
        mdd_node->setChildern(left, right);
      } else {
        mdd_node = MaximalDependenceDecompositionNode::make(node_name,
                                                            model,
                                                            -1);
      }
    }
  }

  return mdd_node;
}

/*----------------------------------------------------------------------------*/

InhomogeneousMarkovChainPtr
MaximalDependenceDecomposition::trainInhomogeneousMarkovChain(
    std::vector<Sequence> & sequences,
    unsigned int alphabet_size) {
  std::vector<VariableLengthMarkovChainPtr> position_specific_vlmcs;

  for (unsigned int j = 0; j < sequences[0].size(); j++) {
    std::vector<Sequence> imc_sequences;

    Sequence s;
    for (unsigned int i = 0; i < sequences.size(); i++) {
      s.push_back(sequences[i][j]);
    }
    imc_sequences.push_back(s);

    auto tree = ContextTree::make(alphabet_size);
    tree->initializeCounter(imc_sequences, 0, {1});
    tree->normalize();
    position_specific_vlmcs.push_back(VariableLengthMarkovChain::make(tree));
  }
  return InhomogeneousMarkovChain::make(position_specific_vlmcs);
}

/*----------------------------------------------------------------------------*/

int MaximalDependenceDecomposition::getMaximalDependenceIndex(
    InhomogeneousMarkovChainPtr model,
    Sequence selected,
    ConsensusSequence consensus_sequence,
    unsigned int alphabet_size,
    ProbabilisticModelPtr consensus_model) {
  Sequence s(consensus_sequence.size(), INVALID_SYMBOL);
  double maximal = -std::numeric_limits<double>::infinity();
  double maximal_i = -1;
  for (unsigned int i = 0; i < consensus_sequence.size(); i++) {
    double sum = -std::numeric_limits<double>::infinity();
    for (unsigned int j = 0; j < consensus_sequence.size(); j++) {
      if (i != j) {
        double chi = -std::numeric_limits<double>::infinity();
        for (unsigned int k = 0; k < alphabet_size; k++) {
          s[i] = k;
          double e = consensus_model->standardEvaluator(s)->evaluateSymbol(i);
          s[j] = k;
          double o = model->standardEvaluator(s)->evaluateSymbol(j);
          double x = (o - e)+(o - e)-e;
          chi = log_sum(chi, x);
        }
        // TODO(igorbonadio): check if 1st parameter is 'maximal' or 'sum' or
        // anything else...
        sum = log_sum(maximal, chi);
      }
    }
    if (maximal < sum) {
      bool ok = false;
      for (unsigned int k = 0; k < selected.size(); k++) {
        if (selected[k] == i) {
          ok = true;
          break;
        }
      }
      if (!ok) {
        maximal = sum;
        maximal_i = i;
      }
    }
  }
  return maximal_i;
}

/*----------------------------------------------------------------------------*/

void MaximalDependenceDecomposition::subset(
    int index,
    std::vector<Sequence> & sequences,
    std::vector<Sequence> & consensus,
    std::vector<Sequence> & nonconsensus,
    ConsensusSequence consensus_sequence) {
  for (unsigned int i = 0; i < sequences.size(); i++) {
    if (consensus_sequence[index].is(sequences[i][index])) {
      consensus.push_back(sequences[i]);
    } else {
      nonconsensus.push_back(sequences[i]);
    }
  }
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*==============================  EVALUATOR  =================================*/

void MaximalDependenceDecomposition::initializeCache(CEPtr<Standard> evaluator,
                                                     unsigned int phase) {
  int slen = evaluator->sequence().size();
  int clen = _consensus_sequence.size();

  if (slen - clen + 1 <= 0) return;

  auto &prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(slen - clen + 1);

  auto simple_evaluator = static_cast<SEPtr<Standard>>(evaluator);

  for (int i = 0; i < slen - clen + 1; i++)
    prefix_sum_array[i]
      = evaluateSequence(simple_evaluator, i, i + clen, phase);
}

/*----------------------------------------------------------------------------*/

Probability
MaximalDependenceDecomposition::evaluateSymbol(SEPtr<Standard> /* evaluator */,
                                               unsigned int /* pos */,
                                               unsigned int /* phase */) const {
  // TODO(igorbonadio)
  return -std::numeric_limits<double>::infinity();
}

/*----------------------------------------------------------------------------*/

Probability
MaximalDependenceDecomposition::evaluateSequence(SEPtr<Standard> evaluator,
                                                 unsigned int begin,
                                                 unsigned int end,
                                                 unsigned int /* phase */) const {
  if ((end - begin) != _consensus_sequence.size())
    return -std::numeric_limits<double>::infinity();
  auto first = evaluator->sequence().begin() + begin;
  auto last = evaluator->sequence().begin() + end;
  Sequence subseq(first, last);
  std::vector<int> indexes;
  return _probabilityOf(subseq, _mdd_tree, indexes);
}

/*----------------------------------------------------------------------------*/

Probability
MaximalDependenceDecomposition::evaluateSequence(CEPtr<Standard> evaluator,
                                                 unsigned int begin,
                                                 unsigned int end,
                                                 unsigned int /* phase */) const {
  auto &prefix_sum_array = evaluator->cache().prefix_sum_array;
  if ((end - begin) != _consensus_sequence.size())
    return -std::numeric_limits<double>::infinity();
  return prefix_sum_array[begin];
}

/*==============================  GENERATOR  =================================*/

Standard<Symbol>
MaximalDependenceDecomposition::drawSymbol(SGPtr<Standard> /* generator */,
                                           unsigned int /* pos */,
                                           unsigned int /* phase */,
                                           const Sequence &/* context */) const {
  return Standard<Symbol>(INVALID_SYMBOL); // TODO(igorbonadio)
}

/*----------------------------------------------------------------------------*/

Standard<Sequence>
MaximalDependenceDecomposition::drawSequence(SGPtr<Standard> /* generator */,
                                             unsigned int size,
                                             unsigned int /* phase */) const {
  // _drawAux(s, _mdd_tree);
  return Sequence(size, INVALID_SYMBOL);
}

/*----------------------------------------------------------------------------*/
/*                             CONCRETE METHODS                               */
/*----------------------------------------------------------------------------*/

/*================================  OTHERS  ==================================*/

double MaximalDependenceDecomposition::_probabilityOf(
    const Sequence & s,
    MaximalDependenceDecompositionNodePtr node,
    std::vector<int> &indexes) const {
  double p = 0;
  if (node->getLeft()) {
    p = node->getModel()
      ->standardEvaluator(s)->evaluateSymbol(node->getIndex());
    indexes.push_back(node->getIndex());
    if (_consensus_sequence[node->getIndex()].is(s[node->getIndex()])) {
      p += _probabilityOf(s, node->getLeft(), indexes);
    } else {
      p += _probabilityOf(s, node->getRight(), indexes);
    }
  } else {  // leaf
    for (unsigned int i = 0; i < s.size(); i++) {
      if (std::find(indexes.begin(), indexes.end(), i) == indexes.end()) {
        p += node->getModel()->standardEvaluator(s)->evaluateSymbol(i);
      }
    }
  }
  return p;
}

/*----------------------------------------------------------------------------*/

void MaximalDependenceDecomposition::_drawAux(
    Sequence &s,
    MaximalDependenceDecompositionNodePtr node) const {
  if (node->getLeft()) {
    s[node->getIndex()]
      = node->getModel()->standardGenerator()->drawSymbol(node->getIndex(), 0, s);
    if (_consensus_sequence[node->getIndex()].is(s[node->getIndex()])) {
      _drawAux(s, node->getLeft());
    } else {
      _drawAux(s, node->getRight());
    }
  } else {  // leaf
    for (unsigned int i = 0; i < s.size(); i++) {
      if (s[i] == INVALID_SYMBOL) {
        s[i] = node->getModel()->standardGenerator()->drawSymbol(i, 0, s);
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
