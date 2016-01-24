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
#include <limits>
#include <vector>

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/Sequence.hpp"
#include "model/Probability.hpp"
#include "model/FixedSequenceAtPosition.hpp"

#include "helper/Sequence.hpp"

// Tested header
#include "model/DiscreteIIDModel.hpp"
#include "helper/DiscreteIIDModel.hpp"

/*----------------------------------------------------------------------------*/
/*                             USING DECLARATIONS                             */
/*----------------------------------------------------------------------------*/

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::ContainerEq;

using tops::model::Sequence;
using tops::model::Probability;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::FixedSequenceAtPosition;
using tops::model::FixedSequenceAtPositionPtr;

using tops::helper::createLoadedCoinIIDModel;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class ADiscreteIIDModelWithFixedSequenceAtPosition : public testing::Test {
 protected:
  FixedSequenceAtPositionPtr iid = FixedSequenceAtPosition::make(
      createLoadedCoinIIDModel(),
      3,
      Sequence{ 1, 0, 1 },
      DiscreteIIDModel::make(
        std::vector<Probability>{
          0, -std::numeric_limits<double>::infinity() }));
};

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(ADiscreteIIDModelWithFixedSequenceAtPosition, ShouldEvaluateSequence) {
  ASSERT_THAT(iid->standardEvaluator({0, 0, 0, 0, 0, 0, 0, 0})
                 ->evaluateSequence(0, 8),
              DoubleEq(-std::numeric_limits<double>::infinity()));
  ASSERT_THAT(iid->standardEvaluator({0, 0, 0, 1, 0, 0, 0, 0})
                 ->evaluateSequence(0, 8),
              DoubleEq(-std::numeric_limits<double>::infinity()));
  ASSERT_THAT(iid->standardEvaluator({0, 0, 0, 1, 1, 0, 0, 0})
                 ->evaluateSequence(0, 8),
              DoubleEq(-std::numeric_limits<double>::infinity()));
  ASSERT_THAT(iid->standardEvaluator({0, 0, 0, 1, 0, 1, 0, 0})
                 ->evaluateSequence(0, 8),
              DoubleNear(-10.1029, 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST_F(ADiscreteIIDModelWithFixedSequenceAtPosition,
    ShouldDrawSequenceWithDefaultSeed) {
  ASSERT_THAT(iid->standardGenerator()->drawSequence(5),
              ContainerEq(Sequence{0, 1, 1, 1, 0}));
}

/*----------------------------------------------------------------------------*/
