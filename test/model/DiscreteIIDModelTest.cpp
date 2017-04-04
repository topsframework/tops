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
#include <utility>

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/Sequence.hpp"
#include "model/Probability.hpp"

#include "helper/Sequence.hpp"
#include "helper/SExprTranslator.hpp"

// Tested header
#include "model/DiscreteIIDModel.hpp"
#include "helper/DiscreteIIDModel.hpp"

// Macros
#define DOUBLE(X) static_cast<double>(X)

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

using tops::helper::SExprTranslator;
using tops::helper::sequenceOfLengths;
using tops::helper::generateRandomSequence;
using tops::helper::createLoadedCoinIIDModel;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class ADiscreteIIDModel : public testing::Test {
 protected:
  DiscreteIIDModelPtr iid = createLoadedCoinIIDModel();
};

/*----------------------------------------------------------------------------*/
/*                                SIMPLE TESTS                                */
/*----------------------------------------------------------------------------*/

TEST(DiscreteIIDModel, ShouldBeTrainedUsingMLAlgorithm) {
  std::vector<Sequence> training_set = {
    {0, 0, 0, 1, 1},
    {0, 0, 0, 1, 0, 0, 1, 1},
    {0, 0, 0, 1, 1, 0, 0},
  };

  auto iid_trainer = DiscreteIIDModel::standardTrainer();
  iid_trainer->add_training_set(std::move(training_set));

  auto iid = iid_trainer->train(
    DiscreteIIDModel::maximum_likehood_algorithm{}, 2);

  ASSERT_THAT(DOUBLE(iid->probabilityOf(0)), DoubleEq(13.0/20));
  ASSERT_THAT(DOUBLE(iid->probabilityOf(1)), DoubleEq(7.0/20));
}

/*----------------------------------------------------------------------------*/

TEST(DiscreteIIDModel, ShouldBeTrainedUsingSmoothedHistogramBurgeAlgorithm) {
  auto iid_trainer = DiscreteIIDModel::standardTrainer();
  iid_trainer->add_training_sequence(sequenceOfLengths());

  auto iid = iid_trainer->train(
    DiscreteIIDModel::smoothed_histogram_burge_algorithm{}, 1.0, 15000);

  ASSERT_THAT(DOUBLE(iid->probabilityOf(4186)), DoubleNear(0.000061, 1e-04));
  ASSERT_THAT(DOUBLE(iid->probabilityOf(3312)), DoubleNear(0.000067, 1e-04));
}

/*----------------------------------------------------------------------------*/

TEST(DiscreteIIDModel, ShouldBeTrainedUsingSmoothedHistogramStankeAlgorithm) {
  auto iid_trainer = DiscreteIIDModel::standardTrainer();
  iid_trainer->add_training_sequence(sequenceOfLengths());

  auto iid = iid_trainer->train(
    DiscreteIIDModel::smoothed_histogram_stanke_algorithm{},
    std::vector<unsigned int>{1}, 15000, 8, 0.5);

  ASSERT_THAT(DOUBLE(iid->probabilityOf(4186)), DoubleNear(0.000059, 1e-04));
  ASSERT_THAT(DOUBLE(iid->probabilityOf(3312)), DoubleNear(0.000059, 1e-04));
}

/*----------------------------------------------------------------------------*/

TEST(DiscreteIIDModel,
    ShouldBeTrainedUsingSmoothedHistogramKernelDensityAlgorithm) {
  auto iid_trainer = DiscreteIIDModel::standardTrainer();
  iid_trainer->add_training_sequence(sequenceOfLengths());

  auto iid = iid_trainer->train(
    DiscreteIIDModel::smoothed_histogram_kernel_density_algorithm{}, 15000);

  ASSERT_THAT(DOUBLE(iid->probabilityOf(4186)), DoubleNear(0.000059, 1e-04));
  ASSERT_THAT(DOUBLE(iid->probabilityOf(3312)), DoubleNear(0.000037, 1e-04));
}

/*----------------------------------------------------------------------------*/

TEST(DiscreteIIDModel, ShouldBeTrainedUsingMLAlgorithmWithEmptyDataSet) {
  auto iid = DiscreteIIDModel::standardTrainer(
               DiscreteIIDModel::maximum_likehood_algorithm{},
               2)->train();

  ASSERT_THAT(DOUBLE(iid->probabilityOf(4186)), DoubleEq(0));
  ASSERT_THAT(DOUBLE(iid->probabilityOf(3312)), DoubleEq(0));
}

/*----------------------------------------------------------------------------*/

TEST(DiscreteIIDModel,
    ShouldBeTrainedUsingSmoothedHistogramBurgeAlgorithmWithEmptyDataSet) {
  auto iid = DiscreteIIDModel::standardTrainer(
               DiscreteIIDModel::smoothed_histogram_burge_algorithm{},
               1.0, 15000)->train();

  ASSERT_THAT(DOUBLE(iid->probabilityOf(4186)), DoubleEq(0));
  ASSERT_THAT(DOUBLE(iid->probabilityOf(3312)), DoubleEq(0));
}

/*----------------------------------------------------------------------------*/

TEST(DiscreteIIDModel,
    ShouldBeTrainedUsingSmoothedHistogramStankeAlgorithmWithAnEmptyDataSet) {
  auto iid = DiscreteIIDModel::standardTrainer(
               DiscreteIIDModel::smoothed_histogram_stanke_algorithm{},
               std::vector<unsigned int>{1}, 15000, 8, 0.5)->train();

  ASSERT_THAT(DOUBLE(iid->probabilityOf(4186)), DoubleEq(0));
  ASSERT_THAT(DOUBLE(iid->probabilityOf(3312)), DoubleEq(0));
}

/*----------------------------------------------------------------------------*/

TEST(DiscreteIIDModel,
ShouldBeTrainedUsingSmoothedHistogramKernelDensityAlgorithmWithAnEmptyDataSet) {
  auto iid = DiscreteIIDModel::standardTrainer(
               DiscreteIIDModel::smoothed_histogram_kernel_density_algorithm{},
               15000)->train();

  ASSERT_THAT(DOUBLE(iid->probabilityOf(4186)), DoubleEq(0));
  ASSERT_THAT(DOUBLE(iid->probabilityOf(3312)), DoubleEq(0));
}

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(ADiscreteIIDModel, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(iid->alphabetSize(), Eq(2));
}

/*----------------------------------------------------------------------------*/

TEST_F(ADiscreteIIDModel, ShouldEvaluateASingleSymbol) {
  ASSERT_THAT(DOUBLE(iid->probabilityOf(0)), DoubleEq(0.2));
  ASSERT_THAT(DOUBLE(iid->probabilityOf(1)), DoubleEq(0.8));
}

/*----------------------------------------------------------------------------*/

TEST_F(ADiscreteIIDModel, ShouldEvaluateASequence) {
  std::vector<Sequence> test_data = {
    {0, 0, 1, 1},
    {0, 1, 1, 1},
    {1, 1, 1, 1}
  };
  for (const auto& data : test_data) {
    Probability result = 1.0;
    for (auto symbol : data)
      result *= iid->probabilityOf(symbol);

    ASSERT_THAT(DOUBLE(iid->standardEvaluator(data)->evaluateSequence(0, 4)),
                DoubleEq(result));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ADiscreteIIDModel, ShouldEvaluateASequencePosition) {
  auto evaluator = iid->standardEvaluator({0, 1, 0});
  ASSERT_THAT(DOUBLE(evaluator->evaluateSequence(0, 1)), DoubleEq(0.2));
  ASSERT_THAT(DOUBLE(evaluator->evaluateSequence(1, 2)), DoubleEq(0.8));
  ASSERT_THAT(DOUBLE(evaluator->evaluateSequence(2, 3)), DoubleEq(0.2));
}

/*----------------------------------------------------------------------------*/

TEST_F(ADiscreteIIDModel, ShouldEvaluateASequenceWithPrefixSumArray) {
  for (int i = 1; i < 1000; i++) {
    auto data = generateRandomSequence(i, 2);
    auto size = data.size();
    ASSERT_THAT(
      DOUBLE(iid->standardEvaluator(data, true)->evaluateSequence(0, size)),
      DoubleEq(iid->standardEvaluator(data)->evaluateSequence(0, size)));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ADiscreteIIDModel, ShouldDrawSequenceWithDefaultSeed) {
  ASSERT_THAT(iid->standardGenerator()->drawSequence(5),
              ContainerEq(Sequence{0, 1, 1, 1, 1}));
}

/*----------------------------------------------------------------------------*/

TEST_F(ADiscreteIIDModel, ShouldBeSExprSerialized) {
  auto translator = SExprTranslator::make();
  auto serializer = iid->serializer(translator);
  serializer->serialize();
  ASSERT_EQ(translator->sexpr(), "(DiscreteIIDModel: 0.200000 0.800000)");
}

/*----------------------------------------------------------------------------*/
