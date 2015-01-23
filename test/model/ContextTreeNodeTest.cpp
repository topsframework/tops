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

#include "gmock/gmock.h"

#include "ContextTreeNode.hpp"
#include "DiscreteIIDModel.hpp"

using ::testing::Eq;
using tops::model::ContextTreeNode;
using tops::model::ContextTreeNodePtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;

class AContextTreeNode : public testing::Test {
 protected:
  DiscreteIIDModelPtr distribution = DiscreteIIDModelPtr(
    new DiscreteIIDModel({0.5, 0.3, 0.2}));
  ContextTreeNodePtr node = ContextTreeNodePtr(
    new ContextTreeNode(distribution));
};

TEST_F(AContextTreeNode, ShouldHaveADistribution) {
  ASSERT_THAT(node->distribution()->_self, Eq(distribution->_self));
}

TEST_F(AContextTreeNode, ShouldHaveChildren) {
  auto c0 = node->child(ContextTreeNodePtr(
    new ContextTreeNode(distribution)), 0);
  auto c1 = node->child(ContextTreeNodePtr(
    new ContextTreeNode(distribution)), 1);
  auto c2 = node->child(ContextTreeNodePtr(
    new ContextTreeNode(distribution)), 2);
  ASSERT_THAT(node->child(0)->_self, Eq(c0->_self));
  ASSERT_THAT(node->child(1)->_self, Eq(c1->_self));
  ASSERT_THAT(node->child(2)->_self, Eq(c2->_self));
}
