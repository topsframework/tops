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

#include "ContextTree.hpp"

namespace tops {
namespace model {

ContextTree::ContextTree(int alphabet_size) {
  auto alphabet = tops::AlphabetPtr(new Alphabet());
  for (int i = 0; i < alphabet_size; i++) {
    alphabet->createSymbol(std::to_string(i));
  }
  _self = tops::ContextTreePtr(new tops::ContextTree(alphabet));
}

ContextTreeNodePtr ContextTree::root() const {
  auto child = ContextTreeNodePtr(
    new ContextTreeNode(DiscreteIIDModelPtr(new DiscreteIIDModel({0}))));
  child->_self = _self->getRoot();
  return child;
}

ContextTreeNodePtr ContextTree::createContext(DiscreteIIDModelPtr iid) {
  auto child = ContextTreeNodePtr(
    new ContextTreeNode(DiscreteIIDModelPtr(new DiscreteIIDModel({0}))));
  child->_self = _self->createContext();
  child->distribution(iid);
  return child;
}

ContextTreeNodePtr ContextTree::context(int context_id) const {
  auto child = ContextTreeNodePtr(
    new ContextTreeNode(DiscreteIIDModelPtr(new DiscreteIIDModel({0}))));
  child->_self = _self->getContext(context_id);
  return child;
}

ContextTreeNodePtr ContextTree::context(const Sequence & s, int index) const {
  auto child = ContextTreeNodePtr(
    new ContextTreeNode(DiscreteIIDModelPtr(new DiscreteIIDModel({0}))));
  child->_self = _self->getContext(s, index);
  return child;
}

}  // namespace model
}  // namespace tops
