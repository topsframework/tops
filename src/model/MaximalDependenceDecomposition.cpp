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
#include "model/MaximalDependenceDecomposition.hpp"

// Standard headers
#include <cmath>
#include <limits>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>

// Internal headers
#include "model/Util.hpp"

#include "exception/NotYetImplemented.hpp"

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

/*================================  TRAINER  =================================*/

MaximalDependenceDecompositionPtr
MaximalDependenceDecomposition::train(TrainerPtr<Multiple, Self> trainer,
                                      standard_training_algorithm,
                                      size_t alphabet_size,
                                      ConsensusSequence consensus_sequence,
                                      ProbabilisticModelPtr consensus_model,
                                      size_t minimum_subset) {
  return MaximalDependenceDecomposition::make(
    consensus_sequence,
    consensus_model,
    trainTree(trainer->training_set(),
              minimum_subset,
              alphabet_size,
              consensus_sequence,
              consensus_model));
}

/*=================================  OTHERS  =================================*/

MaximalDependenceDecompositionNodePtr MaximalDependenceDecomposition::trainTree(
    const std::vector<Multiple<Sequence>>& training_set,
    int divmin,
    size_t alphabet_size,
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
    const std::vector<Multiple<Sequence>>& sequences,
    size_t divmin,
    Sequence selected,
    size_t alphabet_size,
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
    Probability prob = consensus_model
      ->standardEvaluator({ s })->evaluateSymbol(consensus_index);
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
      std::vector<Multiple<Sequence>> consensus_set;
      std::vector<Multiple<Sequence>> non_consensus_set;

      subset(consensus_index,
             sequences,
             consensus_set,
             non_consensus_set,
             consensus_sequence);

      if ((consensus_set.size() > divmin)
          && (non_consensus_set.size() > divmin)) {
        mdd_node = MaximalDependenceDecompositionNode::make(node_name,
                                                            model,
                                                            consensus_index);
        std::stringstream p;
        p << node_name << "_p" << consensus_index;
        MaximalDependenceDecompositionNodePtr left = newNode(
          p.str(),
          consensus_set,
          divmin,
          selected,
          alphabet_size,
          consensus_sequence,
          consensus_model);
        std::stringstream n;
        n << node_name << "_n" << consensus_index;
        MaximalDependenceDecompositionNodePtr right = newNode(
          n.str(),
          non_consensus_set,
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
    const std::vector<Multiple<Sequence>>& training_set,
    size_t alphabet_size) {
  std::vector<VariableLengthMarkovChainPtr> position_specific_vlmcs;

  for (size_t j = 0; j < training_set[0].size(); j++) {
    std::vector<Multiple<Sequence>> imc_sequences;

    Sequence s;
    for (size_t i = 0; i < training_set.size(); i++) {
      s.push_back(training_set[i][0][j]);
    }
    imc_sequences.push_back({ s });

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
    size_t alphabet_size,
    ProbabilisticModelPtr consensus_model) {
  // TODO(renatocf): refactor to use 'Probability'
  Sequence s(consensus_sequence.size(), INVALID_SYMBOL);
  double maximal = -std::numeric_limits<double>::infinity();
  double maximal_i = -1;
  for (size_t i = 0; i < consensus_sequence.size(); i++) {
    double sum = -std::numeric_limits<double>::infinity();
    for (size_t j = 0; j < consensus_sequence.size(); j++) {
      if (i != j) {
        double chi = -std::numeric_limits<double>::infinity();
        for (size_t k = 0; k < alphabet_size; k++) {
          s[i] = k;
          double e
            = consensus_model->standardEvaluator({ s })->evaluateSymbol(i);
          s[j] = k;
          double o
            = model->standardEvaluator({ s })->evaluateSymbol(j);
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
      for (size_t k = 0; k < selected.size(); k++) {
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
    size_t index,
    const std::vector<Multiple<Sequence>>& training_set,
    std::vector<Multiple<Sequence>>& consensus_set,
    std::vector<Multiple<Sequence>>& non_consensus_set,
    const ConsensusSequence& consensus_sequence) {
  for (size_t i = 0; i < training_set.size(); i++) {
    if (consensus_sequence[index].is(training_set[i][0][index])) {
      consensus_set.push_back(training_set[i]);
    } else {
      non_consensus_set.push_back(training_set[i]);
    }
  }
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*==============================  EVALUATOR  =================================*/

Probability MaximalDependenceDecomposition::evaluateSymbol(
    SEPtr<Multiple> evaluator,
    size_t pos,
    size_t phase) const {
  return evaluateSequence(evaluator, pos, pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability MaximalDependenceDecomposition::evaluateSequence(
    SEPtr<Multiple> evaluator,
    size_t begin,
    size_t end,
    size_t /* phase */) const {
  if ((end - begin) != _consensus_sequence.size()) return 0;
  auto first = evaluator->sequence()[0].begin() + begin;
  auto last = evaluator->sequence()[0].begin() + end;
  Sequence subseq(first, last);
  std::vector<int> indexes;
  return _probabilityOf(subseq, _mdd_tree, indexes);
}

/*----------------------------------------------------------------------------*/

void MaximalDependenceDecomposition::initializeCache(CEPtr<Multiple> evaluator,
                                                     size_t phase) {
  int slen = evaluator->sequence()[0].size();
  int clen = _consensus_sequence.size();

  if (slen - clen + 1 <= 0) return;

  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(slen - clen + 1);

  auto simple_evaluator = static_cast<SEPtr<Multiple>>(evaluator);

  for (int i = 0; i < slen - clen + 1; i++)
    prefix_sum_array[i]
      = evaluateSequence(simple_evaluator, i, i + clen, phase);
}

/*----------------------------------------------------------------------------*/

Probability MaximalDependenceDecomposition::evaluateSymbol(
    CEPtr<Multiple> evaluator,
    size_t pos,
    size_t phase) const {
  return Base::evaluateSymbol(evaluator, pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability MaximalDependenceDecomposition::evaluateSequence(
    CEPtr<Multiple> evaluator,
    size_t begin,
    size_t end,
    size_t /* phase */) const {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  if ((end - begin) != _consensus_sequence.size()) return 0;
  return prefix_sum_array[begin];
}

/*==============================  GENERATOR  =================================*/

Multiple<Symbol> MaximalDependenceDecomposition::drawSymbol(
    SGPtr<Multiple> /* generator */,
    size_t /* pos */,
    size_t /* phase */,
    const Multiple<Sequence> &/* context */) const {
  throw_exception(NotYetImplemented);
}

/*----------------------------------------------------------------------------*/

Multiple<Sequence> MaximalDependenceDecomposition::drawSequence(
    SGPtr<Multiple> /* generator */,
    size_t /* size */,
    size_t /* phase */) const {
  throw_exception(NotYetImplemented);
}

/*===============================  SERIALIZER  ===============================*/

void MaximalDependenceDecomposition::serialize(SSPtr serializer) {
  Base::serialize(serializer);
}

/*----------------------------------------------------------------------------*/
/*                             CONCRETE METHODS                               */
/*----------------------------------------------------------------------------*/

/*================================  OTHERS  ==================================*/

Probability MaximalDependenceDecomposition::_probabilityOf(
    const Sequence& s,
    MaximalDependenceDecompositionNodePtr node,
    std::vector<int>& indexes) const {
  Probability p = 1;
  if (node->getLeft()) {
    p = node->getModel()
      ->standardEvaluator({ s })->evaluateSymbol(node->getIndex());
    indexes.push_back(node->getIndex());
    if (_consensus_sequence[node->getIndex()].is(s[node->getIndex()])) {
      p *= _probabilityOf(s, node->getLeft(), indexes);
    } else {
      p *= _probabilityOf(s, node->getRight(), indexes);
    }
  } else {  // leaf
    for (size_t i = 0; i < s.size(); i++) {
      if (std::find(indexes.begin(), indexes.end(), i) == indexes.end()) {
        p *= node->getModel()->standardEvaluator({ s })->evaluateSymbol(i);
      }
    }
  }
  return p;
}

/*----------------------------------------------------------------------------*/

void MaximalDependenceDecomposition::_drawAux(
    Multiple<Sequence>& s,
    MaximalDependenceDecompositionNodePtr node) const {
  if (node->getLeft()) {
    s[node->getIndex()]
      = node->getModel()
        ->standardGenerator()->drawSymbol(node->getIndex(), 0, s);
    if (_consensus_sequence[node->getIndex()].is(s[0][node->getIndex()])) {
      _drawAux(s, node->getLeft());
    } else {
      _drawAux(s, node->getRight());
    }
  } else {  // leaf
    for (size_t i = 0; i < s[0].size(); i++) {
      auto symbols = node->getModel()->standardGenerator()->drawSymbol(i, 0, s);
      for (size_t j = 0; j < s.size(); j++) {
        if (s[j][i] == INVALID_SYMBOL) {
          s[j][i] = symbols[j];
        }
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
