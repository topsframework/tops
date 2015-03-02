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
#include <math.h>
#include <vector>

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/SimilarityBasedSequenceWeighting.hpp"
#include "model/Sequence.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;

using tops::model::SimilarityBasedSequenceWeighting;
using tops::model::SimilarityBasedSequenceWeightingPtr;
using tops::model::Sequence;

class ASBSW : public testing::Test {
 protected:
  SimilarityBasedSequenceWeightingPtr sbsw;

  virtual void SetUp() {
    std::map<Sequence, double> counter;
    counter[{1, 1}] = 2;
    counter[{0, 0}] = 4;
    sbsw = SimilarityBasedSequenceWeighting::make(counter, 6, -1, -1, {1, 0});
  }
};

TEST_F(ASBSW, ShouldEvaluateASequence) {
  ASSERT_THAT(sbsw->evaluateSequence({0}, 0, 1),
              DoubleEq(-HUGE));
  ASSERT_THAT(sbsw->evaluateSequence({1}, 0, 1),
              DoubleEq(-HUGE));
  ASSERT_THAT(sbsw->evaluateSequence({0, 1}, 0, 2),
              DoubleNear(-6.90776, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({0, 0}, 0, 2),
              DoubleNear(-0.405465, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({1, 0}, 0, 2),
              DoubleNear(-6.90776, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({1, 1}, 0, 2),
              DoubleNear(-1.09861, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({1, 0, 1}, 0, 3),
              DoubleNear(-6.90776, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({1, 0, 1, 0}, 0, 4),
              DoubleNear(-6.90776, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 13),
              DoubleNear(-6.90776, 1e-4));
}

TEST_F(ASBSW, ShouldEvaluateAPosition) {
  ASSERT_THAT(sbsw->evaluatePosition({0}, 0), DoubleEq(-HUGE));
  ASSERT_THAT(sbsw->evaluatePosition({1}, 0), DoubleEq(-HUGE));
  ASSERT_THAT(sbsw->evaluatePosition({0, 1}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(sbsw->evaluatePosition({0, 0}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(sbsw->evaluatePosition({1, 0}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(sbsw->evaluatePosition({1, 1}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(sbsw->evaluatePosition({1, 0, 1}, 2), DoubleEq(-HUGE));
  ASSERT_THAT(sbsw->evaluatePosition({1, 0, 1, 0}, 3), DoubleEq(-HUGE));
}

TEST_F(ASBSW, ShouldEvaluateASequenceWithPrefixSumArray) {
  sbsw->initializePrefixSumArray({0});
  ASSERT_THAT(sbsw->evaluateWithPrefixSumArray(0, 1),
              DoubleNear(sbsw->evaluateSequence({0}, 0, 1), 1e-4));

  sbsw->initializePrefixSumArray({0, 1});
  ASSERT_THAT(sbsw->evaluateWithPrefixSumArray(0, 2),
              DoubleNear(sbsw->evaluateSequence({0, 1}, 0, 2), 1e-4));

  sbsw->initializePrefixSumArray({1, 0, 1});
  ASSERT_THAT(sbsw->evaluateWithPrefixSumArray(0, 3),
              DoubleNear(sbsw->evaluateSequence({1, 0, 1}, 0, 3), 1e-4));

  sbsw->initializePrefixSumArray({1, 0, 1, 0});
  ASSERT_THAT(sbsw->evaluateWithPrefixSumArray(0, 4),
              DoubleNear(sbsw->evaluateSequence({1, 0, 1, 0}, 0, 4), 1e-4));

  sbsw->initializePrefixSumArray({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
  ASSERT_THAT(sbsw->evaluateWithPrefixSumArray(0, 13),
              DoubleNear(sbsw->evaluateSequence({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 13), 1e-4));
}

TEST(SBSW, ShouldBeTrained) {
  std::vector<Sequence> training_set = {
    {1, 1},
    {0, 1},
    {1, 1},
    {0, 0},
    {1, 1},
  };
  auto sbsw = SimilarityBasedSequenceWeighting::train(training_set, 2, -1, -1, {});
  ASSERT_THAT(sbsw->evaluateSequence({0}, 0, 1),
              DoubleEq(-HUGE));
  ASSERT_THAT(sbsw->evaluateSequence({1}, 0, 1),
              DoubleEq(-HUGE));
  ASSERT_THAT(sbsw->evaluateSequence({0, 1}, 0, 2),
              DoubleNear(-1.60684, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({0, 0}, 0, 2),
              DoubleNear(-1.60984, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({1, 0}, 0, 2),
              DoubleNear(-7.1323, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({1, 1}, 0, 2),
              DoubleNear(-0.511891, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({1, 0, 1}, 0, 3),
              DoubleNear(-7.1323, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({1, 0, 1, 0}, 0, 4),
              DoubleNear(-7.1323, 1e-4));
  ASSERT_THAT(sbsw->evaluateSequence({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 13),
              DoubleNear(-7.1323, 1e-4));
}
