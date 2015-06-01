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
#include <vector>

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/Segment.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::ContainerEq;

using tops::model::Sequence;
using tops::model::Segment;

TEST(ASegment, ShouldBeGeneratedBySequences) {
  std::vector<Segment> segments;
  segments.push_back(Segment(0, 0, 2));
  segments.push_back(Segment(1, 2, 4));
  segments.push_back(Segment(0, 4, 6));
  Sequence s = {0, 0, 1, 1, 0, 0};
  auto x = Segment::readSequence(s);
  for (unsigned int i = 0; i < segments.size(); i++) {
    ASSERT_THAT(x[i].symbol(), Eq(segments[i].symbol()));
    ASSERT_THAT(x[i].begin(), Eq(segments[i].begin()));
    ASSERT_THAT(x[i].end(), Eq(segments[i].end()));
  }

  segments.clear();
  segments.push_back(Segment(0, 0, 2));
  segments.push_back(Segment(1, 2, 4));
  segments.push_back(Segment(0, 4, 5));
  s = {0, 0, 1, 1, 0};
  x = Segment::readSequence(s);
  for (unsigned int i = 0; i < segments.size(); i++) {
    ASSERT_THAT(x[i].symbol(), Eq(segments[i].symbol()));
    ASSERT_THAT(x[i].begin(), Eq(segments[i].begin()));
    ASSERT_THAT(x[i].end(), Eq(segments[i].end()));
  }

  segments.clear();
  segments.push_back(Segment(0, 0, 2));
  s = {0, 0};
  x = Segment::readSequence(s);
  for (unsigned int i = 0; i < segments.size(); i++) {
    ASSERT_THAT(x[i].symbol(), Eq(segments[i].symbol()));
    ASSERT_THAT(x[i].begin(), Eq(segments[i].begin()));
    ASSERT_THAT(x[i].end(), Eq(segments[i].end()));
  }
}
