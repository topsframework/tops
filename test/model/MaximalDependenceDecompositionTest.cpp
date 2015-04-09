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
#include "helper/DiscreteIIDModel.hpp"

using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::Eq;

using tops::model::MaximalDependenceDecomposition;
using tops::model::MaximalDependenceDecompositionPtr;
using tops::model::Sequence;

using tops::helper::createMDD;
using tops::helper::createDNAModel;
using tops::helper::createConsensusSequence;

class AMDD : public testing::Test {
 protected:
  MaximalDependenceDecompositionPtr mdd = createMDD();
};

TEST_F(AMDD, ShouldEvaluateAPosition) {
  ASSERT_THAT(mdd->evaluate({0}, 0), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluate({1}, 0), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluate({0, 1}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluate({0, 0}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluate({1, 0}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluate({1, 1}, 1), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluate({1, 0, 1}, 2), DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluate({1, 0, 1, 0}, 3), DoubleEq(-HUGE));
}

TEST_F(AMDD, ShouldEvaluateASequence) {
  ASSERT_THAT(mdd->evaluator({0})->probabilityOf(0, 1),
              DoubleEq(-HUGE));
  ASSERT_THAT(mdd->evaluator({1, 0, 2, 2, 3, 2, 0, 0, 3})->probabilityOf(0, 9),
              DoubleNear(-14.0795, 1e-4));
  ASSERT_THAT(mdd->evaluator({1, 2, 2, 2, 3, 2, 0, 2, 3})->probabilityOf(0, 9),
              DoubleNear(-11.3069, 1e-4));
  ASSERT_THAT(mdd->evaluator({2, 2, 2, 2, 2, 2, 2, 2, 2})->probabilityOf(0, 9),
              DoubleNear(-8.24662, 1e-4));
}

TEST_F(AMDD, ShouldEvaluateASequenceWithPrefixSumArray) {
  ASSERT_THAT(mdd->evaluator({0}, true)->probabilityOf(0, 1),
              DoubleEq(mdd->evaluator({0})->probabilityOf(0, 1)));
  ASSERT_THAT(mdd->evaluator({1, 0, 2, 2, 3, 2, 0, 0, 3}, true)->probabilityOf(0, 9),
              DoubleEq(mdd->evaluator({1, 0, 2, 2, 3, 2, 0, 0, 3})->probabilityOf(0, 9)));
  ASSERT_THAT(mdd->evaluator({1, 2, 2, 2, 3, 2, 0, 2, 3}, true)->probabilityOf(0, 9),
              DoubleEq(mdd->evaluator({1, 2, 2, 2, 3, 2, 0, 2, 3})->probabilityOf(0, 9)));
  ASSERT_THAT(mdd->evaluator({2, 2, 2, 2, 2, 2, 2, 2, 2}, true)->probabilityOf(0, 9),
              DoubleEq(mdd->evaluator({2, 2, 2, 2, 2, 2, 2, 2, 2})->probabilityOf(0, 9)));
}

TEST(MDD, ShouldBeTrained) {
  auto consensus_sequence = createConsensusSequence();
  auto consensus_model = createDNAModel();
  std::vector<Sequence> training_set = {{1, 0, 3, 1, 3, 2, 3, 0, 1},
                                        {0, 1, 2, 2, 3, 2, 3, 0, 2},
                                        {1, 0, 3, 2, 3, 1, 0, 0, 2},
                                        {0, 1, 2, 1, 3, 1, 0, 0, 3},
                                        {1, 0, 2, 2, 2, 1, 0, 1, 3},
                                        {0, 1, 3, 2, 3, 2, 0, 1, 3},
                                        {1, 0, 2, 1, 3, 2, 3, 1, 0}};
  auto mdd = MaximalDependenceDecomposition::train(
    training_set,
    4,
    consensus_sequence,
    consensus_model,
    2);
  ASSERT_THAT(mdd->evaluator({1, 0, 2, 2, 3, 2, 0, 0, 3})->probabilityOf(0, 9),
              DoubleNear(-6.45814, 1e-4));
  ASSERT_THAT(mdd->evaluator({1, 1, 2, 2, 3, 2, 0, 0, 3})->probabilityOf(0, 9),
              DoubleNear(-5.765, 1e-4));
  ASSERT_THAT(mdd->evaluator({1, 1, 3, 2, 3, 2, 0, 0, 0})->probabilityOf(0, 9),
              DoubleNear(-6.96784, 1e-4));
}