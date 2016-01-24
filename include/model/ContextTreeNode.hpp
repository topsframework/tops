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

#ifndef TOPS_MODEL_CONTEXT_TREE_NODE_
#define TOPS_MODEL_CONTEXT_TREE_NODE_

// Standard headers
#include <map>
#include <memory>
#include <vector>

// ToPS headers
#include "model/DiscreteIIDModel.hpp"

namespace tops {
namespace model {

// Forward declaration
class ContextTreeNode;

/**
 * @typedef ContextTreeNodePtr
 * @brief Alias of pointer to ContextTreeNode.
 */
using ContextTreeNodePtr = std::shared_ptr<ContextTreeNode>;

/**
 * @class ContextTreeNode
 * @brief Class that represents a node of a context tree.
 */
class ContextTreeNode {
 public:
  // Static methods
  static ContextTreeNodePtr make(int alphabet_size);

  // Concrete methods
  int alphabet_size() const;
  void setParent(int parent);
  int getParent();
  int id();
  void setId(int id);
  void addCount(int s);
  void addCount(int s, double w);
  void setCount(int s, double v);
  std::vector<double>& getCounter();
  void setChild(ContextTreeNodePtr child, int symbol);
  int symbol();
  void setSymbol(int symbol);
  void setDistribution(DiscreteIIDModelPtr distribution);
  ContextTreeNodePtr getChild(int symbol);
  DiscreteIIDModelPtr getDistribution();
  void deleteChildren();
  std::vector<ContextTreeNodePtr> getChildren();
  bool isLeaf();

 private:
  // Instance variables
  DiscreteIIDModelPtr _distribution;
  std::vector<ContextTreeNodePtr> _child;
  int _alphabet_size;
  int _symbol;
  bool _leaf;
  std::vector<double> _counter;
  int _id;
  int _parent_id;

  // Constructors
  explicit ContextTreeNode(int alphabet_size);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_CONTEXT_TREE_NODE_
