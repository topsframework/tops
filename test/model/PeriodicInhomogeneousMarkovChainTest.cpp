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
#include "model/Sequence.hpp"
#include "model/VariableLengthMarkovChain.hpp"
#include "model/ProbabilisticModelDecoratorCrtp.hpp"

#include "helper/Sequence.hpp"
#include "helper/VariableLengthMarkovChain.hpp"

// Tested header
#include "model/PeriodicInhomogeneousMarkovChain.hpp"

/*----------------------------------------------------------------------------*/
/*                             USING DECLARATIONS                             */
/*----------------------------------------------------------------------------*/

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::ContainerEq;

using tops::model::Sequence;
using tops::model::ProbabilisticModelPtr;
using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::ProbabilisticModelDecoratorCrtp;
using tops::model::PeriodicInhomogeneousMarkovChain;
using tops::model::ProbabilisticModelDecoratorCrtpPtr;
using tops::model::PeriodicInhomogeneousMarkovChainPtr;

using tops::helper::createVLMCMC;
using tops::helper::createMachlerVLMC;
using tops::helper::generateRandomSequence;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class APeriodicInhomogeneousMarkovChain : public testing::Test {
 protected:
  PeriodicInhomogeneousMarkovChainPtr imc;

  virtual void SetUp() {
    imc = PeriodicInhomogeneousMarkovChain::make({createMachlerVLMC(),
                                                  createVLMCMC()});
  }
};

/*----------------------------------------------------------------------------*/
/*                                SIMPLE TESTS                                */
/*----------------------------------------------------------------------------*/

TEST(PeriodicInhomogeneousMarkovChain, ShouldBeTrained) {
  auto imc_trainer = PeriodicInhomogeneousMarkovChain::standardTrainer();

  imc_trainer->add_training_set({{1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                 {0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                 {1, 1, 0, 1, 0, 1, 1, 0, 1, 0},
                                 {0, 1, 1, 0, 0, 0, 0, 1, 0, 1}});

  auto imc = imc_trainer->train(
      PeriodicInhomogeneousMarkovChain::interpolation_algorithm{},
      2, 2, 2, 1.5, std::vector<double>{1.0, 1.0, 1.0, 1.0}, nullptr);

  ASSERT_THAT(imc->standardEvaluator({1, 0, 1, 0})->evaluateSequence(0, 4),
              DoubleNear(-2.99504, 1e-4));
  ASSERT_THAT(imc->standardEvaluator({1, 1, 1, 1})->evaluateSequence(0, 4),
              DoubleNear(-2.99504, 1e-4));
  ASSERT_THAT(imc->standardEvaluator({0, 0, 0, 1, 1, 1, 1})
                 ->evaluateSequence(0, 7),
              DoubleNear(-4.87431, 1e-4));
}

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(APeriodicInhomogeneousMarkovChain, ShouldEvaluateASequence) {
  ASSERT_THAT(imc->standardEvaluator({0})->evaluateSequence(0, 1),
              DoubleEq(log(0.50)));
  ASSERT_THAT(imc->standardEvaluator({1})->evaluateSequence(0, 1),
              DoubleEq(log(0.50)));
  ASSERT_THAT(imc->standardEvaluator({0, 1})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.90)));
  ASSERT_THAT(imc->standardEvaluator({0, 0})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.10)));
  ASSERT_THAT(imc->standardEvaluator({1, 0})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.50)));
  ASSERT_THAT(imc->standardEvaluator({1, 1})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.50)));
  ASSERT_THAT(imc->standardEvaluator({1, 0, 1})->evaluateSequence(0, 3),
              DoubleEq(log(0.5) + log(0.5) + log(0.80)));
}

/*----------------------------------------------------------------------------*/

TEST_F(APeriodicInhomogeneousMarkovChain,
       ShouldEvaluateASequenceWithPrefixSumArray) {
  for (int i = 1; i < 1000; i++) {
    auto data = generateRandomSequence(i, 2);
    ASSERT_THAT(imc->standardEvaluator(data, true)
                   ->evaluateSequence(0, data.size()),
                DoubleEq(imc->standardEvaluator(data)
                            ->evaluateSequence(0, data.size())));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APeriodicInhomogeneousMarkovChain, CanBeDecorated) {
  auto decorated_imc
    = std::make_shared<ProbabilisticModelDecoratorCrtp<
                        PeriodicInhomogeneousMarkovChain>>(imc);
  ASSERT_THAT(decorated_imc->standardEvaluator({0})->evaluateSequence(0, 1),
              DoubleEq(log(0.50)));
  ASSERT_THAT(decorated_imc->standardEvaluator({1})->evaluateSequence(0, 1),
              DoubleEq(log(0.50)));
  ASSERT_THAT(decorated_imc->standardEvaluator({0, 1})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.90)));
  ASSERT_THAT(decorated_imc->standardEvaluator({0, 0})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.10)));
  ASSERT_THAT(decorated_imc->standardEvaluator({1, 0})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.50)));
  ASSERT_THAT(decorated_imc->standardEvaluator({1, 1})->evaluateSequence(0, 2),
              DoubleEq(log(0.50) + log(0.50)));
  ASSERT_THAT(decorated_imc->standardEvaluator({1, 0, 1})
                           ->evaluateSequence(0, 3),
              DoubleEq(log(0.5) + log(0.5) + log(0.80)));
}

/*----------------------------------------------------------------------------*/

TEST_F(APeriodicInhomogeneousMarkovChain, ShouldChooseSequenceWithDefaultSeed) {
  // TODO(igorbonadio): check bigger sequence
  ASSERT_THAT(imc->standardGenerator()->drawSequence(5),
              ContainerEq(Sequence{0, 1, 1, 0, 1}));
}

/*----------------------------------------------------------------------------*/
