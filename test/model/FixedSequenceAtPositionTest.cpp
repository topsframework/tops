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
#include "model/DiscreteIIDModel.hpp"
#include "model/FixedSequenceAtPosition.hpp"
#include "model/Sequence.hpp"

#include "helper/DiscreteIIDModel.hpp"
#include "helper/Sequence.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;

using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::FixedSequenceAtPosition;
using tops::model::FixedSequenceAtPositionPtr;
using tops::model::Sequence;

using tops::helper::createLoadedCoinIIDModel;

class ADiscreteIIDModelWithFixedSequenceAtPosition : public testing::Test {
 protected:
  FixedSequenceAtPositionPtr iid = FixedSequenceAtPosition::make(
      createLoadedCoinIIDModel(),
      3,
      {1, 0, 1},
      DiscreteIIDModel::make({0, -HUGE}));
};

TEST_F(ADiscreteIIDModelWithFixedSequenceAtPosition, ShouldEvaluateSequence) {
  ASSERT_THAT(iid->evaluate({0, 0, 0, 0, 0, 0, 0, 0})->probabilityOf(0, 8),
              DoubleEq(-HUGE));
  ASSERT_THAT(iid->evaluate({0, 0, 0, 1, 0, 0, 0, 0})->probabilityOf(0, 8),
              DoubleEq(-HUGE));
  ASSERT_THAT(iid->evaluate({0, 0, 0, 1, 1, 0, 0, 0})->probabilityOf(0, 8),
              DoubleEq(-HUGE));
  ASSERT_THAT(iid->evaluate({0, 0, 0, 1, 0, 1, 0, 0})->probabilityOf(0, 8),
              DoubleNear(-10.1029, 1e-4));
}
