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

#include <iostream>

// ToPS headers
#include "MaximalDependenceDecomposition.hpp"

namespace tops {
namespace model {

MaximalDependenceDecompositionPtr MaximalDependenceDecomposition::make(
    unsigned int alphabet_size,
    ConsensusSequence consensus_sequence,
    ProbabilisticModelPtr consensus_model,
    MaximalDependenceDecompositionNodePtr tree) {
  return MaximalDependenceDecompositionPtr(
    new MaximalDependenceDecomposition(alphabet_size,
                                       consensus_sequence,
                                       consensus_model,
                                       tree));
}

MaximalDependenceDecomposition::MaximalDependenceDecomposition(
    unsigned int alphabet_size,
    ConsensusSequence consensus_sequence,
    ProbabilisticModelPtr consensus_model,
    MaximalDependenceDecompositionNodePtr tree)
      : _mdd_tree(tree),
        _consensus_sequence(consensus_sequence),
        _consensus_model(consensus_model),
        _alphabet_size(alphabet_size) {
}

int MaximalDependenceDecomposition::alphabetSize() const {
  return _alphabet_size;
}

double MaximalDependenceDecomposition::evaluateSequence(
    const Sequence &s,
    unsigned int begin,
    unsigned int end) const {
  if ((end - begin) != _consensus_sequence.size())
    return -HUGE;
  auto first = s.begin() + begin;
  auto last = s.begin() + end;
  Sequence subseq(first, last);
  std::vector<int> indexes;
  return _evaluateAux(subseq, _mdd_tree, indexes);
}

double MaximalDependenceDecomposition::_evaluateAux(const Sequence & s, MaximalDependenceDecompositionNodePtr node, std::vector<int> &indexes) const {
  double p = 0;
  if (node->getLeft()) {
    p = node->getModel()->evaluatePosition(s, node->getIndex());
    indexes.push_back(node->getIndex());
    // cout << node->getIndex() << endl;
    // cout << "tem filho" << endl;
    if (_consensus_sequence[node->getIndex()].is(s[node->getIndex()])) {
      // cout << "eh consensus" << endl;
      p += _evaluateAux(s, node->getLeft(), indexes);
    } else {
      // cout << "nao eh consensus" << endl;
      p += _evaluateAux(s, node->getRight(), indexes);
    }
  } else { // leaf
    // cout << "nao tem filho" << endl;
    for (int i = 0; i < s.size(); i++) {
      if (std::find(indexes.begin(), indexes.end(), i) == indexes.end()) {
        p += node->getModel()->evaluatePosition(s, i);
      }
    }
  }
  return p;
}

double MaximalDependenceDecomposition::evaluatePosition(const Sequence &s, unsigned int i) const {
  // TODO
  return -HUGE;
}

Sequence MaximalDependenceDecomposition::chooseSequence(Sequence &s, unsigned int size) const {
  s = Sequence(size, -1);
  _chooseAux(s, _mdd_tree);
  return s;
}

void MaximalDependenceDecomposition::_chooseAux(Sequence & s, MaximalDependenceDecompositionNodePtr node) const {
  if (node->getLeft()) {
    s[node->getIndex()] = node->getModel()->choosePosition(s, node->getIndex());
    if (_consensus_sequence[node->getIndex()].is(s[node->getIndex()])) {
      _chooseAux(s, node->getLeft());
    } else {
      _chooseAux(s, node->getRight());
    }
  } else { // leaf
    for (int i = 0; i < s.size(); i++) {
      if (s[i] == -1) {
        s[i] = node->getModel()->choosePosition(s, i);
      }
    }
  }
}

Symbol MaximalDependenceDecomposition::choosePosition(const Sequence &s, unsigned int i) const {
  // TODO
  return 0;
}

}  // namespace model
}  // namespace tops
