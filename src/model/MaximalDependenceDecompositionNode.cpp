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

// Interface header
#include "model/MaximalDependenceDecompositionNode.hpp"

// Standard headers
#include <cmath>
#include <string>
#include <vector>

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

MaximalDependenceDecompositionNode::MaximalDependenceDecompositionNode(
    std::string node_name,
    ProbabilisticModelPtr model,
    int index)
      : _model(model),
        _index(index),
        _node_name(node_name) {
}

/*----------------------------------------------------------------------------*/
/*                              STATIC METHODS                                */
/*----------------------------------------------------------------------------*/

MaximalDependenceDecompositionNodePtr MaximalDependenceDecompositionNode::make(
    std::string node_name,
    ProbabilisticModelPtr model,
    int index) {
  return MaximalDependenceDecompositionNodePtr(
    new MaximalDependenceDecompositionNode(node_name, model, index));
}

/*----------------------------------------------------------------------------*/
/*                             CONCRETE METHODS                               */
/*----------------------------------------------------------------------------*/

int MaximalDependenceDecompositionNode::getIndex() {
  return _index;
}

/*----------------------------------------------------------------------------*/

ProbabilisticModelPtr MaximalDependenceDecompositionNode::getModel() {
  return _model;
}

/*----------------------------------------------------------------------------*/

MaximalDependenceDecompositionNodePtr
MaximalDependenceDecompositionNode::getLeft() {
  return _left;
}

/*----------------------------------------------------------------------------*/

MaximalDependenceDecompositionNodePtr
MaximalDependenceDecompositionNode::getRight() {
  return _right;
}

/*----------------------------------------------------------------------------*/

void MaximalDependenceDecompositionNode::setChildern(
    MaximalDependenceDecompositionNodePtr left,
    MaximalDependenceDecompositionNodePtr right) {
  _left = left;
  _right = right;
}

/*----------------------------------------------------------------------------*/

void MaximalDependenceDecompositionNode::setChild(
    MaximalDependenceDecompositionNodePtr child) {
  _left = child;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
