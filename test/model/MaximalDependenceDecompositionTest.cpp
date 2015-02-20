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
#include "model/MaximalDependenceDecomposition.hpp"
#include "model/Sequence.hpp"

#include "helper/MaximalDependenceDecomposition.hpp"

using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::Eq;

using tops::model::MaximalDependenceDecomposition;
using tops::model::MaximalDependenceDecompositionPtr;
using tops::model::Sequence;

using tops::helper::createMDD;

class AMDD : public testing::Test {
 protected:
  MaximalDependenceDecompositionPtr mdd = createMDD();
};

TEST_F(AMDD, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(mdd->alphabetSize(), Eq(2));
}

TEST_F(AMDD, ShouldEvaluateAPosition) {
  ASSERT_THAT(mdd->evaluatePosition({0}, 0), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluatePosition({1}, 0), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluatePosition({0, 1}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluatePosition({0, 0}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluatePosition({1, 0}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluatePosition({1, 1}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluatePosition({1, 0, 1}, 2), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluatePosition({1, 0, 1, 0}, 3), DoubleEq(-HUGE));
}

TEST_F(AMDD, ShouldEvaluateASequence) {
  ASSERT_THAT(mdd->evaluateSequence({0}, 0, 1),
              DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluateSequence({1, 0, 2, 2, 3, 2, 0, 0, 3}, 0, 9),
              DoubleNear(-14.0795, 1e-4));
  ASSERT_THAT(mdd->evaluateSequence({1, 2, 2, 2, 3, 2, 0, 2, 3}, 0, 9),
              DoubleNear(-11.3069, 1e-4));
  ASSERT_THAT(mdd->evaluateSequence({2, 2, 2, 2, 2, 2, 2, 2, 2}, 0, 9),
              DoubleNear(-8.24662, 1e-4));
}