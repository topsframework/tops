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

#include <math.h>

#include "gmock/gmock.h"

#include "HiddenMarkovModelState.hpp"
#include "DiscreteIIDModel.hpp"


using ::testing::Eq;
using tops::model::HiddenMarkovModelState;
using tops::model::HiddenMarkovModelStatePtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;

class AHMMState : public testing::Test {
 protected:
  DiscreteIIDModelPtr emission = DiscreteIIDModelPtr(new DiscreteIIDModel({0.5, 0.3, 0.1, 0.1}));
  DiscreteIIDModelPtr transition = DiscreteIIDModelPtr(new DiscreteIIDModel({0.6, 0.4}));
  HiddenMarkovModelStatePtr state = HiddenMarkovModelStatePtr(new HiddenMarkovModelState(0, emission, transition));
};

TEST_F(AHMMState, ShouldEmitSymbols) {
  ASSERT_THAT(state->emissions()->logProbabilityOf(0), Eq(log(0.5)));
  ASSERT_THAT(state->emissions()->logProbabilityOf(1), Eq(log(0.3)));
  ASSERT_THAT(state->emissions()->logProbabilityOf(2), Eq(log(0.1)));
  ASSERT_THAT(state->emissions()->logProbabilityOf(3), Eq(log(0.1)));
}

TEST_F(AHMMState, ShouldTransit) {
  ASSERT_THAT(state->transitions()->logProbabilityOf(0), Eq(log(0.6)));
  ASSERT_THAT(state->transitions()->logProbabilityOf(1), Eq(log(0.4)));
}
