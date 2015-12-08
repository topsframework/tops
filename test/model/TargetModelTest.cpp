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
#include "model/TargetModel.hpp"
#include "model/Sequence.hpp"

using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::Eq;
using ::testing::ContainerEq;

using tops::model::TargetModel;
using tops::model::TargetModelPtr;
using tops::model::Sequence;

class ATargetModel : public testing::Test {
 protected:
  TargetModelPtr target = TargetModel::make(2);
};

TEST_F(ATargetModel, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(target->alphabetSize(), Eq(2));
}

TEST_F(ATargetModel, ShouldEvaluateASingleSymbol) {
  ASSERT_THAT(target->probabilityOf(0), DoubleEq(log(0.5)));
  ASSERT_THAT(target->probabilityOf(1), DoubleEq(log(0.5)));
}

TEST_F(ATargetModel, ShouldHaveEvaluateASequence) {
  ASSERT_THAT(
    target->standardEvaluator({0, 1, 0})->evaluateSequence(0, 3),
    DoubleEq(log(2.0/3.0) + log(1.0/3.0) + log(2.0/3.0)));

  ASSERT_THAT(
    target->standardEvaluator({0, 1, 1})->evaluateSequence(0, 3),
    DoubleEq(log(1.0/3.0) + log(2.0/3.0) + log(2.0/3.0)));

  ASSERT_THAT(
    target->standardEvaluator({0, 1, 1, 1})->evaluateSequence(0, 4),
    DoubleEq(log(1.0/4.0) + log(3.0/4.0) + log(3.0/4.0) + log(3.0/4.0)));
}

TEST_F(ATargetModel, ShouldEvaluateASequenceWithPrefixSumArray) {
  ASSERT_THAT(
    target->standardEvaluator({0, 1, 0}, true)->evaluateSequence(0, 3),
    DoubleEq(target->standardEvaluator({0, 1, 0})->evaluateSequence(0, 3)));

  ASSERT_THAT(
    target->standardEvaluator({0, 1, 1})->evaluateSequence(0, 3),
    DoubleEq(target->standardEvaluator({0, 1, 1}, true)
                   ->evaluateSequence(0, 3)));

  ASSERT_THAT(
    target->standardEvaluator({0, 1, 1, 1}, true)->evaluateSequence(0, 4),
    DoubleEq(target->standardEvaluator({0, 1, 1, 1})->evaluateSequence(0, 4)));
}

TEST_F(ATargetModel, ShouldDrawSequence) {
  // TODO(igorbonadio): check bigger sequence
  ASSERT_THAT(target->standardGenerator()->drawSequence(5),
              ContainerEq(Sequence{0, 1, 1, 0, 0}));
}
