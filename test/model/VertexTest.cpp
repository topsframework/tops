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

#include "Vertex.hpp"


using ::testing::Eq;
using tops::model::Vertex;
using tops::model::VertexPtr;

class AVertex : public testing::Test {
 protected:
  VertexPtr v1 = VertexPtr(new Vertex(0));
  VertexPtr v2 = VertexPtr(new Vertex(1));
  VertexPtr v3 = VertexPtr(new Vertex(2));
  VertexPtr v4 = VertexPtr(new Vertex(3));

  virtual void SetUp() {
    v1->connect(v1);
    v1->connect(v2);
    v1->connect(v3);
    v1->connect(v4);

    v2->connect(v2);
    v2->connect(v3);
    v2->connect(v4);

    v3->connect(v3);
    v3->connect(v4);

    v4->connect(v4);
  }
};

TEST_F(AVertex, ShouldHaveNextVertexes) {
  ASSERT_THAT(v1->nextVertexes().size(), Eq(4));
  ASSERT_THAT(v2->nextVertexes().size(), Eq(3));
  ASSERT_THAT(v3->nextVertexes().size(), Eq(2));
  ASSERT_THAT(v4->nextVertexes().size(), Eq(1));
}

TEST_F(AVertex, ShouldHavePreviousVertexes) {
  ASSERT_THAT(v4->previousVertexes().size(), Eq(4));
  ASSERT_THAT(v3->previousVertexes().size(), Eq(3));
  ASSERT_THAT(v2->previousVertexes().size(), Eq(2));
  ASSERT_THAT(v1->previousVertexes().size(), Eq(1));
}
