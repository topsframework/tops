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

#include <cstdlib>
#include <vector>

#include "ContextTreeNode.hpp"

namespace tops {
namespace model {

ContextTreeNodePtr ContextTreeNode::make(int alphabet_size) {
  return ContextTreeNodePtr(new ContextTreeNode(alphabet_size));
}

ContextTreeNode::ContextTreeNode(int alphabet_size) {
  _alphabet_size = alphabet_size;
  _child.resize(_alphabet_size);
  _counter.resize(_alphabet_size);
  for (int i = 0; i < static_cast<int>(_counter.size()); i++)
    _counter[i] = 0;
  _symbol = -1;
  _leaf = true;
  _id = 0;
}

int ContextTreeNode::alphabet_size() const {
  return _alphabet_size;
}

void ContextTreeNode::setParent(int parent) {
  _parent_id = parent;
}

int ContextTreeNode::getParent() {
  return _parent_id;
}

int ContextTreeNode::id() {
  return _id;
}

void ContextTreeNode::setId(int id) {
  _id = id;
}

void ContextTreeNode::addCount(int s) {
  _counter[s] += 1.0;
}

void ContextTreeNode::addCount(int s, double weight) {
  _counter[s] += weight;
}


void ContextTreeNode::setCount(int s, double v) {
  _counter[s] = v;
}

std::vector<double>& ContextTreeNode::getCounter() {
  return _counter;
}

void ContextTreeNode::setChild(ContextTreeNodePtr child, int symbol) {
  // assert((symbol >= 0) && (symbol < (int)_child.size()));
  _child[symbol] = child;
  child->setSymbol(symbol);
  child->setParent(id());
  _leaf = false;
}

int ContextTreeNode::symbol() {
  return _symbol;
}

void ContextTreeNode::setSymbol(int symbol) {
  _symbol = symbol;
}

void ContextTreeNode::setDistribution(DiscreteIIDModelPtr distribution) {
  _distribution = distribution;
}

ContextTreeNodePtr ContextTreeNode::getChild(int symbol) {
  if (!((symbol >= 0) && (symbol < static_cast<int>(_child.size())))) {
    exit(-1);
    return NULL;
  }
  return _child[symbol];
}

DiscreteIIDModelPtr ContextTreeNode::getDistribution() {
  return _distribution;
}

void ContextTreeNode::deleteChildren() {
  ContextTreeNodePtr n;
  for (int m = 0; m < static_cast<int>(_child.size()); m++)
    if (_child[m] != NULL)
      _child[m]->setParent(-1);
  _child.resize(0);
  _child.resize(_alphabet_size);
  _leaf = true;
}
std::vector <ContextTreeNodePtr> ContextTreeNode::getChildren() {
  return _child;
}

bool ContextTreeNode::isLeaf() {
  return _leaf;
}

}  // namespace model
}  // namespace tops
