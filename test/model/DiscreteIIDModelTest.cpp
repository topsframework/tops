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
#include <vector>

#include "gmock/gmock.h"

#include "DiscreteIIDModel.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::Sequence;

class ADiscreteIIDModel : public testing::Test {
 protected:
  DiscreteIIDModelPtr model = DiscreteIIDModelPtr(
    new DiscreteIIDModel({0.5, 0.3, 0.2}));
};

TEST_F(ADiscreteIIDModel, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(model->alphabetSize(), Eq(3));
}

TEST_F(ADiscreteIIDModel, ShouldHaveEvaluateASingleSymbol) {
  ASSERT_THAT(model->logProbabilityOf(0), DoubleEq(log(0.5)));
  ASSERT_THAT(model->logProbabilityOf(1), DoubleEq(log(0.3)));
  ASSERT_THAT(model->logProbabilityOf(2), DoubleEq(log(0.2)));
}

TEST_F(ADiscreteIIDModel, ShouldHaveEvaluateASequence) {
  std::vector<Sequence> test_data = {
    {0, 0, 1, 1, 2, 2},
    {0, 1, 2, 2, 2, 2},
    {1, 1, 1, 0, 0, 0}
  };
  for (auto data : test_data) {
    double result = 0.0;
    for (auto symbol : data) {
      result += model->logProbabilityOf(symbol);
    }
    ASSERT_THAT(model->evaluate(data, 0, 5), DoubleEq(result));
  }
}

TEST_F(ADiscreteIIDModel, ShouldHaveEvaluateASequencePosition) {
  ASSERT_THAT(model->evaluatePosition({2, 1, 0}, 0), DoubleEq(log(0.2)));
  ASSERT_THAT(model->evaluatePosition({2, 1, 0}, 1), DoubleEq(log(0.3)));
  ASSERT_THAT(model->evaluatePosition({2, 1, 0}, 2), DoubleEq(log(0.5)));
}
