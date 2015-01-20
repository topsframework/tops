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

ContextTreeNode::ContextTreeNode(DiscreteIIDModelPtr iid) {
  _self = tops::ContextTreeNodePtr(
    new tops::ContextTreeNode(iid->alphabetSize()));
  this->distribution(iid);
}

DiscreteIIDModelPtr ContextTreeNode::distribution(DiscreteIIDModelPtr iid) {
  _self->setDistribution(
    boost::static_pointer_cast<tops::DiscreteIIDModel>(iid->_self));
  return iid;
}

DiscreteIIDModelPtr ContextTreeNode::distribution() {
  auto iid = DiscreteIIDModelPtr(new DiscreteIIDModel({}));
  iid->_self = _self->getDistribution();
  return iid;
}

ContextTreeNodePtr ContextTreeNode::child(
    ContextTreeNodePtr child,
    Symbol symbol) {
  _self->setChild(child->_self, symbol);
  return child;
}

ContextTreeNodePtr ContextTreeNode::child(Symbol symbol) {
  auto child = ContextTreeNodePtr(
    new ContextTreeNode(DiscreteIIDModelPtr(new DiscreteIIDModel({0}))));
  child->_self = _self->getChild(symbol);
  return child;
}

}  // namespace model
}  // namespace tops
