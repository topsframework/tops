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

#include "ContextTreeNode.hpp"

namespace tops {
namespace model {

ContextTreeNodePtr ContextTreeNode::make(DiscreteIIDModelPtr distribution) {
  return ContextTreeNodePtr(new ContextTreeNode(distribution));
}

ContextTreeNode::ContextTreeNode(DiscreteIIDModelPtr distribution): _leaf(true), _distribution(distribution) {
}

int ContextTreeNode::alphabetSize() const {
  return _distribution->alphabetSize();
}

bool ContextTreeNode::isLeaf() {
  return _leaf;
}

ContextTreeNodePtr ContextTreeNode::addChild(int symbol, DiscreteIIDModelPtr distribution) {
  auto node = ContextTreeNodePtr(new ContextTreeNode(distribution));
  _children[symbol] = node;
  _leaf = false;
  return node;
}

ContextTreeNodePtr ContextTreeNode::getChild(int symbol) {
  return _children[symbol];
}

DiscreteIIDModelPtr ContextTreeNode::getDistribution() {
  return _distribution;
}

}
}
