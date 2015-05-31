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
#include <set>
#include <cmath>
#include <vector>
#include <cstdlib>

// ToPS headers
#include "model/ContextTree.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                              CONSTRUCTORS                                  */
/*----------------------------------------------------------------------------*/

ContextTree::ContextTree(int alphabet_size)
    : _alphabet_size(alphabet_size) {
}

/*----------------------------------------------------------------------------*/
/*                              STATIC METHODS                                */
/*----------------------------------------------------------------------------*/

ContextTreePtr ContextTree::make(int alphabet_size) {
  return ContextTreePtr(new ContextTree(alphabet_size));
}

/*----------------------------------------------------------------------------*/
/*                             CONCRETE METHODS                               */
/*----------------------------------------------------------------------------*/

std::vector<ContextTreeNodePtr> & ContextTree::all_context() {
  return _all_context;
}

ContextTreeNodePtr ContextTree::getRoot() const {
    return _all_context[0];
  }

ContextTreeNodePtr ContextTree::createContext() {
  ContextTreeNodePtr n = ContextTreeNode::make(_alphabet_size);
  n->setId(_all_context.size());
  if (n->id() == 0)
      n->setParent(0);
  _all_context.push_back(n);
  return n;
}

ContextTreeNodePtr ContextTree::getContext(int id) {
  return _all_context[id];
}

//! get the context for the sequence s[i-1], s[i-2], s[i-3]...
ContextTreeNodePtr ContextTree::getContext(const Sequence & s, int i) {
  ContextTreeNodePtr c = _all_context[0];
  ContextTreeNodePtr p;
  int j;
  for (j = i-1; j >=0; j--) {
    if (c->isLeaf())
      break;
    p = c;
    c = c->getChild(s[j]);
    if (c == NULL) {
      c = p;
      break;
    }
  }
  return c;
}

std::set<int> ContextTree::getLevelOneNodes() {
  std::set<int> result;
  for (int i = 0; i  < static_cast<int>(_all_context.size()); i++) {
    if (_all_context[i]->isLeaf()) {
      int parent_id = _all_context[i]->getParent();
      if (parent_id < 0)
        continue;
      ContextTreeNodePtr parent = _all_context[parent_id];
      bool levelOne = true;
      for (int l = 0; l < static_cast<int>(_alphabet_size); l++) {
        if ((parent->getChild(l) != NULL) && !parent->getChild(l)->isLeaf())
          levelOne = false;
      }
      if (levelOne)
        result.insert(parent->id());
    }
  }
  return result;
}

void ContextTree::removeContextNotUsed() {
  std::vector <ContextTreeNodePtr> newAllVector;
  for (int i = 0; i  < static_cast<int>(_all_context.size()); i++) {
    if (_all_context[i]->getParent() >= 0) {
      _all_context[i]->setId(newAllVector.size());
      newAllVector.push_back(_all_context[i]);
      if ((_all_context[i] != NULL) && (!_all_context[i]->isLeaf())) {
        for (int  m = 0; m < static_cast<int>(_alphabet_size); m++)
          _all_context[i]->getChild(m)->setParent(_all_context[i]->id());
      }
    }
  }
  _all_context = newAllVector;
}

void ContextTree::normalize() {
  for (int i = 0; i  < static_cast<int>(_all_context.size()); i++) {
    double total = 0;
    std::vector<double> probs(_alphabet_size);
    for (int l = 0; l < static_cast<int>(_alphabet_size); l++)
      total += static_cast<double>((_all_context[i]->getCounter())[l]);
    for (int l = 0; l < static_cast<int>(_alphabet_size); l++)
      probs[l] = log(
        static_cast<double>((_all_context[i]->getCounter())[l])/total);
    DiscreteIIDModelPtr distr = DiscreteIIDModel::make(probs);
    _all_context[i]->setDistribution(distr);
  }
}

void ContextTree::normalize(ProbabilisticModelPtr old, double pseudocount) {
  if (old == NULL) {
      exit(-1);
  }
  for (int i = 0; i  < static_cast<int>(_all_context.size()); i++) {
    double total = 0;
    std::vector<double> probs(_alphabet_size);

    Sequence s;
    ContextTreeNodePtr current = _all_context[i];
    bool valid = true;
    while (current != getRoot()) {
      s.push_back(current->symbol());
      if (current->getParent() < 0) {
        valid = false;
        break;
      }
      current = _all_context[current->getParent()];
    }
    if (!valid)
      continue;

    for (int l = 0; l < static_cast<int>(_alphabet_size); l++)
      total += static_cast<double>((_all_context[i]->getCounter())[l]);

    for (int l = 0; l < static_cast<int>(_alphabet_size); l++) {
      Sequence s3;
      s3 = s;
      s3.push_back(l);

      double prob = exp(old->evaluate(s3, s3.size()-1));

      probs[l] = log(static_cast<double>((_all_context[i]->getCounter())[l]
                 + pseudocount*prob)/(total + pseudocount));
    }

    DiscreteIIDModelPtr distr = DiscreteIIDModel::make(probs);
    _all_context[i]->setDistribution(distr);
  }
}

void ContextTree::initializeCounter(const std::vector<Sequence> & sequences,
                                  int order,
                                  const std::vector<double> &weights) {
  initializeCounter(sequences, order, 0, weights);
}

void ContextTree::initializeCounter(const std::vector<Sequence> & sequences,
                                    int order,
                                    double pseudocounts,
                                    const std::vector<double> &weights) {
  if (order < 0)
    order = 0;

  ContextTreeNodePtr root = createContext();
  if (pseudocounts > 0) {
    for (int sym = 0; sym < root->alphabet_size(); sym++) {
      root->setCount(sym, pseudocounts);
    }
  }

  for (int l = 0; l < static_cast<int>(sequences.size()); l ++) {
    double weight = weights[l];
    for (int i = order; i < static_cast<int>(sequences[l].size()); i++) {
      int currentSymbol = sequences[l][i];
      int j = i - 1;

      ContextTreeNodePtr w = getRoot();

      w->addCount(currentSymbol, weight);

      while ((j >= 0) &&  ((i - j) <= order)) {
        int symbol = sequences[l][j];
        if ((w->getChild(symbol) == NULL) || w->isLeaf()) {
          ContextTreeNodePtr c2 = createContext();
          w->setChild(c2, symbol);
        }
        w = w->getChild(symbol);

        if (pseudocounts > 0) {
          for (int sym = 0; sym < root->alphabet_size(); sym++) {
            if (w->getCounter()[sym] <= 0.0)
              w->setCount(sym, pseudocounts);
          }
        }

        w->addCount(currentSymbol, weight);
        j--;
      }
    }
  }
}

void ContextTree::pruneTreeSmallSampleSize(int small_) {
  std::set<int> x = getLevelOneNodes();
  std::vector<int> nodesToPrune(x.begin(), x.end());

  while (nodesToPrune.size() > 0) {
    int id = nodesToPrune.back();
    nodesToPrune.pop_back();
    double total = 0.0;
    ContextTreeNodePtr parentNode = getContext(id);
    if (parentNode->isLeaf())
      break;

    for (int m = 0; m < static_cast<int>(_alphabet_size); m++)
      total += (parentNode->getCounter())[m];
    for (int l = 0; l < static_cast<int>(_alphabet_size); l++) {
      ContextTreeNodePtr childNode = parentNode->getChild(l);
      if (childNode == NULL)
        continue;
      double totalchild = 0;
      for (int m = 0; m < static_cast<int>(_alphabet_size); m++) {
        totalchild += (childNode->getCounter())[m];
      }
      if (totalchild < small_) {
        for (int m = 0; m < static_cast<int>(_alphabet_size); m++) {
          (childNode->getCounter())[m] = (parentNode->getCounter())[m];
        }
      }
    }
    if (total < small_) {
      parentNode->deleteChildren();
      ContextTreeNodePtr parentNode2 = getContext(parentNode->getParent());
      bool toPrune = true;
      for (int l = 0; l < static_cast<int>(_alphabet_size); l++) {
        if ((parentNode2->getChild(l) != NULL)
            && !(parentNode2->getChild(l)->isLeaf())) {
          toPrune = false;
          break;
        }
      }
      if (toPrune)
        nodesToPrune.push_back(parentNode2->id());
    }
  }
}

void ContextTree::pruneTree(double delta) {
  double sample_size = 0.0;
  for (int l = 0; l < static_cast<int>(_alphabet_size); l++)
    sample_size += (getRoot()->getCounter())[l];
  std::set<int> x = getLevelOneNodes();
  std::vector<int> nodesToPrune(x.begin(), x.end());
  double small_ = (static_cast<double>(_alphabet_size))*log(sample_size);

  while (nodesToPrune.size() > 0) {
    int id = nodesToPrune.back();
    nodesToPrune.pop_back();
    double total = 0.0;
    double total_diff = 0.0;
    ContextTreeNodePtr parentNode = getContext(id);
    if (parentNode->isLeaf())
      break;

    for (int m = 0; m < static_cast<int>(_alphabet_size); m++)
      total += (parentNode->getCounter())[m];
    bool foundSmall = false;
    for (int l = 0; l < static_cast<int>(_alphabet_size); l++) {
      ContextTreeNodePtr childNode = parentNode->getChild(l);
      double totalChild = 0.0;
      for (int m = 0; m < static_cast<int>(_alphabet_size); m++)
        totalChild+= (childNode->getCounter())[m];
      for (int m = 0; m < static_cast<int>(_alphabet_size); m++) {
        double diff = static_cast<double>((parentNode->getCounter())[m])/total;
        diff -= static_cast<double>((childNode->getCounter())[m]) /totalChild;
        if (static_cast<double>((childNode->getCounter())[m]) < small_) {
          foundSmall = true;
          break;
        }
        if (diff < 0)
          total_diff -= diff;
        else
          total_diff += diff;
      }
      if (foundSmall)
        break;
    }
    if ((total < small_) || (total_diff < delta) || (foundSmall == true)) {
      parentNode->deleteChildren();
      ContextTreeNodePtr parentNode2 = getContext(parentNode->getParent());
      bool toPrune = true;
      for (int l = 0; l < static_cast<int>(_alphabet_size); l++) {
        if ((parentNode2->getChild(l) != NULL)
            && !(parentNode2->getChild(l)->isLeaf())) {
          toPrune = false;
          break;
        }
      }
      if (toPrune)
        nodesToPrune.push_back(parentNode2->id());
    }
  }
}

void ContextTree::initializeContextTreeRissanen(
    const std::vector<Sequence> & sequences) {
  ContextTreeNodePtr root = createContext();
  for (int i = 0; i < static_cast<int>(_alphabet_size); i++)
    root->addCount(i);

  for (int s = 0; s < static_cast<int>(sequences.size()); s++) {
    for (int i = 0; i < static_cast<int>(sequences[s].size()); i++) {
      int v = sequences[s][i];
      ContextTreeNodePtr w = root;
      if ((!w->isLeaf()) && ((w->getCounter())[v] == 1.0)) {
        for (int l = 0; l < static_cast<int>(_alphabet_size); l++) {
          ContextTreeNodePtr n = w->getChild(l);
          n->addCount(v);
        }
        w->addCount(v);
        continue;
      }
      if (w->isLeaf() && ((w->getCounter())[v] == 1.0)) {
        for (int l = 0; l < static_cast<int>(_alphabet_size); l++) {
          ContextTreeNodePtr n = createContext();
          n->addCount(v);
          w->setChild(n, l);
        }
        w->addCount(v);
        continue;
      }
      int j = i - 1;
      if (j < 0)
        w->addCount(v);
      while (j >= 0) {
        int u = sequences[s][j];
        w->addCount(v);
        w = w->getChild(u);
        if ((!w->isLeaf()) && (w->getCounter())[v] == 1.0) {
          for (int l = 0; l < static_cast<int>(_alphabet_size); l++) {
            ContextTreeNodePtr n = w->getChild(l);
            n->addCount(v);
          }
          w->addCount(v);
          break;
        }
        if (w->isLeaf() && ((w->getCounter())[v] == 1.0)) {
          for (int l = 0; l  < static_cast<int>(_alphabet_size); l++) {
            ContextTreeNodePtr n = createContext();
            n->addCount(v);
            w->setChild(n, l);
          }
          w->addCount(v);
          break;
        }
        j = j-1;
      }
    }
  }
}

int ContextTree::getNumberOfNodes() const {
  return _all_context.size();
}

int ContextTree::alphabetSize() const {
  return _alphabet_size;
}

}  // namespace model
}  // namespace tops
