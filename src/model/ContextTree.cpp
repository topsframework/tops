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

ContextTreePtr ContextTree::make(ContextTreeNodePtr root) {
  return ContextTreePtr(new ContextTree(root));
}

ContextTree::ContextTree(ContextTreeNodePtr root): _root(root) {
}

int ContextTree::alphabetSize() const {
  return _root->alphabetSize();
}

ContextTreeNodePtr ContextTree::getContext(Sequence sequence, int i) {
  ContextTreeNodePtr c = _root;
  ContextTreeNodePtr p;
  for (int j = i-1; j >= 0; j--) {
    if (c->isLeaf())
      return c;
    p = c;
    c = c->getChild(sequence[j]);
    if (!c)
      return p;
  }
  return c;
}

}  // namespace model
}  // namespace tops
