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

#ifndef TOPS_MODEL_MAXIMAL_DEPENDENCE_DECOMPOSITION_NODE_
#define TOPS_MODEL_MAXIMAL_DEPENDENCE_DECOMPOSITION_NODE_

// Standard headers
#include <memory>
#include <vector>
#include <string>

// ToPS headers
#include "ProbabilisticModel.hpp"

namespace tops {
namespace model {

class MaximalDependenceDecompositionNode;

/**
 * @typedef MaximalDependenceDecompositionNodePtr
 * @brief Alias of pointer to MaximalDependenceDecompositionNode.
 */
using MaximalDependenceDecompositionNodePtr = std::shared_ptr<MaximalDependenceDecompositionNode>;

/**
 * @class MaximalDependenceDecompositionNode
 * @brief TODO
 */
class MaximalDependenceDecompositionNode {
 public:
  static MaximalDependenceDecompositionNodePtr make(std::string node_name, ProbabilisticModelPtr model, int index);

  int getIndex();
  ProbabilisticModelPtr getModel();

  void setChildern(MaximalDependenceDecompositionNodePtr left, MaximalDependenceDecompositionNodePtr right);
  void setChild(MaximalDependenceDecompositionNodePtr child);
  MaximalDependenceDecompositionNodePtr getLeft();
  MaximalDependenceDecompositionNodePtr getRight();

 private:
  MaximalDependenceDecompositionNode(std::string node_name, ProbabilisticModelPtr model, int index);
  std::vector<int> _otherIndexes;
  ProbabilisticModelPtr _model;
  int _index;
  std::string _node_name;
  MaximalDependenceDecompositionNodePtr _left;
  MaximalDependenceDecompositionNodePtr _right;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_MAXIMAL_DEPENDENCE_DECOMPOSITION_NODE_
