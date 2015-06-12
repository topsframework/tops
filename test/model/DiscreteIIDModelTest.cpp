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
#include "model/Sequence.hpp"
#include "model/Random.hpp"

#include "helper/DiscreteIIDModel.hpp"
#include "helper/Sequence.hpp"

// ToPS templates
#include "model/ProbabilisticModelDecorator.tcc"

using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::Eq;
using ::testing::ContainerEq;

using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::ProbabilisticModelDecorator;
using tops::model::ProbabilisticModelDecoratorPtr;
using tops::model::Sequence;

using tops::helper::createLoadedCoinIIDModel;
using tops::helper::generateRandomSequence;
using tops::helper::sequenceOfLengths;

class ADiscreteIIDModel : public testing::Test {
 protected:
  DiscreteIIDModelPtr iid = createLoadedCoinIIDModel();
};

TEST_F(ADiscreteIIDModel, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(iid->alphabetSize(), Eq(2));
}

TEST_F(ADiscreteIIDModel, ShouldEvaluateASingleSymbol) {
  ASSERT_THAT(iid->probabilityOf(0), DoubleEq(log(0.2)));
  ASSERT_THAT(iid->probabilityOf(1), DoubleEq(log(0.8)));
}

TEST_F(ADiscreteIIDModel, ShouldHaveEvaluateASequence) {
  std::vector<Sequence> test_data = {
    {0, 0, 1, 1},
    {0, 1, 1, 1},
    {1, 1, 1, 1}
  };
  for (auto data : test_data) {
    double result = 0.0;
    for (auto symbol : data) {
      result += iid->probabilityOf(symbol);
    }
    ASSERT_THAT(iid->evaluator(data)->probabilityOf(0, 4), DoubleEq(result));
  }
}

TEST_F(ADiscreteIIDModel, ShouldEvaluateASequencePosition) {
  auto evaluator = iid->evaluator({0, 1, 0});
  ASSERT_THAT(evaluator->probabilityOf(0, 1), DoubleEq(log(0.2)));
  ASSERT_THAT(evaluator->probabilityOf(1, 2), DoubleEq(log(0.8)));
  ASSERT_THAT(evaluator->probabilityOf(2, 3), DoubleEq(log(0.2)));
}

TEST_F(ADiscreteIIDModel, ShouldEvaluateASequenceWithPrefixSumArray) {
  for (int i = 1; i < 1000; i++) {
    auto data = generateRandomSequence(i, 2);
    ASSERT_THAT(iid->evaluator(data, true)->probabilityOf(0, data.size()),
                DoubleEq(iid->evaluator(data)->probabilityOf(0, data.size())));
  }
}

TEST_F(ADiscreteIIDModel, ShouldChooseSequenceWithSeed42) {
  tops::model::resetRandom();
  ASSERT_THAT(iid->standardGenerator()->chooseSequence(5), ContainerEq(Sequence{1, 0, 1, 1, 1}));
}

TEST(DiscreteIIDModel, ShouldBeTrainedUsingMLAlgorithm) {
  std::vector<Sequence> training_set = {
    {0, 0, 0, 1, 1},
    {0, 0, 0, 1, 0, 0, 1, 1},
    {0, 0, 0, 1, 1, 0, 0},
  };
  auto iid = DiscreteIIDModel::trainML(training_set, 2);
  ASSERT_THAT(iid->probabilityOf(0), DoubleEq(log(13.0/20)));
  ASSERT_THAT(iid->probabilityOf(1), DoubleEq(log(7.0/20)));
}

TEST(DiscreteIIDModel, ShouldBeTrainedUsingSmoothedHistogramBurgeAlgorithm) {
  auto training_set = {sequenceOfLengths()};
  auto iid = DiscreteIIDModel::trainSmoothedHistogramBurge(training_set,
                                                           1.0,
                                                           15000);
  ASSERT_THAT(iid->probabilityOf(4186), DoubleNear(-9.70443, 1e-04));
  ASSERT_THAT(iid->probabilityOf(3312), DoubleNear(-9.60564, 1e-04));
}

TEST(DiscreteIIDModel, ShouldBeTrainedUsingSmoothedHistogramStankeAlgorithm) {
  auto training_set = {sequenceOfLengths()};
  auto iid = DiscreteIIDModel::trainSmoothedHistogramStanke(training_set,
                                                            {1},
                                                            15000,
                                                            8,
                                                            0.5);
  ASSERT_THAT(iid->probabilityOf(4186), DoubleNear(-9.9706, 1e-04));
  ASSERT_THAT(iid->probabilityOf(3312), DoubleNear(-9.73428, 1e-04));
}

TEST(DiscreteIIDModel,
    ShouldBeTrainedUsingSmoothedHistogramKernelDensityAlgorithm) {
  auto training_set = {sequenceOfLengths()};
  auto iid = DiscreteIIDModel::trainSmoothedHistogramKernelDensity(
      training_set, 15000);
  ASSERT_THAT(iid->probabilityOf(4186), DoubleNear(-9.72518, 1e-04));
  ASSERT_THAT(iid->probabilityOf(3312), DoubleNear(-10.1987, 1e-04));
}

TEST(DiscreteIIDModel, ShouldBeTrainedUsingMLAlgorithmWithEmptyDataSet) {
  auto iid = DiscreteIIDModel::trainML({}, 2);
  ASSERT_THAT(iid->probabilityOf(4186), DoubleEq(-HUGE));
  ASSERT_THAT(iid->probabilityOf(3312), DoubleEq(-HUGE));
}

TEST(DiscreteIIDModel, ShouldBeTrainedUsingSmoothedHistogramBurgeAlgorithmWithEmptyDataSet) {
  auto iid = DiscreteIIDModel::trainSmoothedHistogramBurge({},
                                                           1.0,
                                                           15000);
  ASSERT_THAT(iid->probabilityOf(4186), DoubleEq(-HUGE));
  ASSERT_THAT(iid->probabilityOf(3312), DoubleEq(-HUGE));
}

TEST(DiscreteIIDModel, ShouldNotBeTrainedUsingSmoothedHistogramStankeAlgorithmWithAnEmptyDataSet) {
  auto iid = DiscreteIIDModel::trainSmoothedHistogramStanke({},
                                                            {1},
                                                            15000,
                                                            8,
                                                            0.5);
  ASSERT_THAT(iid->probabilityOf(4186), DoubleEq(-HUGE));
  ASSERT_THAT(iid->probabilityOf(3312), DoubleEq(-HUGE));
}

TEST(DiscreteIIDModel,
    ShouldBeTrainedUsingSmoothedHistogramKernelDensityAlgorithmWithAnEmptyDataSet) {
  auto iid = DiscreteIIDModel::trainSmoothedHistogramKernelDensity(
      {}, 15000);
  ASSERT_THAT(iid->probabilityOf(4186), DoubleEq(-HUGE));
  ASSERT_THAT(iid->probabilityOf(3312), DoubleEq(-HUGE));
}
