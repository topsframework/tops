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

#ifndef CONTEXT_TREE_HPP_
#define CONTEXT_TREE_HPP_

#include <memory>

#include "ContextTreeNode.hpp"

#include "src/ContextTree.hpp"

namespace tops {
namespace model {

class ContextTree {
 public:
  explicit ContextTree(int alphabet_size);

  ContextTreeNodePtr root() const;

  ContextTreeNodePtr createContext(DiscreteIIDModelPtr iid);

  ContextTreeNodePtr context(int context_id) const;
  ContextTreeNodePtr context(const Sequence & s, int index) const;

  // After refactoring, remove _self!
  tops::ContextTreePtr _self;
};

typedef std::shared_ptr<ContextTree> ContextTreePtr;

}  // namespace model
}  // namespace tops

#endif
