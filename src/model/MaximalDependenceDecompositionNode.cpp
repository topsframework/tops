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
#include <string>
#include <vector>

// ToPS headers
#include "MaximalDependenceDecompositionNode.hpp"

namespace tops {
namespace model {

MaximalDependenceDecompositionNodePtr MaximalDependenceDecompositionNode::make(
    std::string node_name,
    ProbabilisticModelPtr model,
    int index) {
  return MaximalDependenceDecompositionNodePtr(
    new MaximalDependenceDecompositionNode(node_name, model, index));
}

MaximalDependenceDecompositionNode::MaximalDependenceDecompositionNode(
    std::string node_name,
    ProbabilisticModelPtr model,
    int index)
      : _model(model),
        _index(index),
        _node_name(node_name) {
}

int MaximalDependenceDecompositionNode::getIndex() {
  return _index;
}

ProbabilisticModelPtr MaximalDependenceDecompositionNode::getModel() {
  return _model;
}

void MaximalDependenceDecompositionNode::setChildern(
    MaximalDependenceDecompositionNodePtr left,
    MaximalDependenceDecompositionNodePtr right) {
  _left = left;
  _right = right;
}

void MaximalDependenceDecompositionNode::setChild(
    MaximalDependenceDecompositionNodePtr child) {
  _left = child;
}

MaximalDependenceDecompositionNodePtr
MaximalDependenceDecompositionNode::getLeft() {
  return _left;
}

MaximalDependenceDecompositionNodePtr
MaximalDependenceDecompositionNode::getRight() {
  return _right;
}

}  // namespace model
}  // namespace tops
