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

#include "ContextTree.hpp"
#include "DiscreteIIDModel.hpp"

using ::testing::Eq;
using tops::model::ContextTree;
using tops::model::ContextTreePtr;
using tops::model::ContextTreeNode;
using tops::model::ContextTreeNodePtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;

class AContextTree : public testing::Test {
 protected:
  DiscreteIIDModelPtr d1 = DiscreteIIDModelPtr(
    new DiscreteIIDModel({0.5, 0.3, 0.2}));
  ContextTreeNodePtr n1;
  DiscreteIIDModelPtr d2 = DiscreteIIDModelPtr(
    new DiscreteIIDModel({0.4, 0.4, 0.3}));
  ContextTreeNodePtr n2;
  DiscreteIIDModelPtr d3 = DiscreteIIDModelPtr(
    new DiscreteIIDModel({0.3, 0.3, 0.4}));
  ContextTreeNodePtr n3;
  DiscreteIIDModelPtr d4 = DiscreteIIDModelPtr(
    new DiscreteIIDModel({0.7, 0.2, 0.1}));
  ContextTreeNodePtr n4;

  ContextTreePtr tree = ContextTreePtr(new ContextTree(3));

  virtual void SetUp() {
    n1 = tree->createContext(d1);
    n2 = tree->createContext(d2);
    n3 = tree->createContext(d3);
    n4 = tree->createContext(d4);

    n1->child(n2, 0);
    n1->child(n3, 1);
    n3->child(n4, 0);
  }
};

TEST_F(AContextTree, ShouldHaveARootNode) {
  ASSERT_THAT(tree->root()->_self, Eq(n1->_self));
}

TEST_F(AContextTree, ShouldHaveARootNodeThatHasChildren) {
  ASSERT_THAT(tree->root()->child(0)->_self, Eq(n2->_self));
  ASSERT_THAT(tree->root()->child(1)->_self, Eq(n3->_self));
  ASSERT_THAT(tree->root()->child(1)->child(0)->_self, Eq(n4->_self));
}
