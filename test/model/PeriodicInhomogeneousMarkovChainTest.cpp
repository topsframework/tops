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

#include "helper/Sequence.hpp"
#include "helper/VariableLengthMarkovChain.hpp"

// Tested header
#include "model/PeriodicInhomogeneousMarkovChain.hpp"

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
using tops::model::ProbabilisticModelPtr;
using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::PeriodicInhomogeneousMarkovChain;
using tops::model::PeriodicInhomogeneousMarkovChainPtr;

using tops::helper::createVLMCMC;
using tops::helper::createMachlerVLMC;
using tops::helper::generateRandomSequence;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class APeriodicIMC: public testing::Test {
 protected:
  PeriodicInhomogeneousMarkovChainPtr pimc;

  virtual void SetUp() {
    pimc = PeriodicInhomogeneousMarkovChain::make(
      std::vector<VariableLengthMarkovChainPtr>{
        createMachlerVLMC(), createVLMCMC()});
  }
};

/*----------------------------------------------------------------------------*/
/*                                SIMPLE TESTS                                */
/*----------------------------------------------------------------------------*/

TEST(PeriodicIMC, ShouldBeTrained) {
  auto pimc_trainer = PeriodicInhomogeneousMarkovChain::standardTrainer();

  pimc_trainer->add_training_set({{{1, 0, 1, 0, 1, 0, 1, 0, 1, 0}},
                                  {{0, 1, 0, 1, 0, 1, 0, 1, 0, 1}},
                                  {{1, 1, 0, 1, 0, 1, 1, 0, 1, 0}},
                                  {{0, 1, 1, 0, 0, 0, 0, 1, 0, 1}}});

  auto pimc = pimc_trainer->train(
      PeriodicInhomogeneousMarkovChain::interpolation_algorithm{},
      2, 2, 2, 1.5, std::vector<double>{1.0, 1.0, 1.0, 1.0}, nullptr);

  ASSERT_THAT(DOUBLE(pimc->standardEvaluator({{1, 0, 1, 0}})
                         ->evaluateSequence(0, 4)),
              DoubleNear(0.050034, 1e-4));
  ASSERT_THAT(DOUBLE(pimc->standardEvaluator({{1, 1, 1, 1}})
                         ->evaluateSequence(0, 4)),
              DoubleNear(0.050034, 1e-4));
  ASSERT_THAT(DOUBLE(pimc->standardEvaluator({{0, 0, 0, 1, 1, 1, 1}})
                  ->evaluateSequence(0, 7)),
              DoubleNear(0.007640, 1e-4));
}

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(APeriodicIMC, ShouldEvaluateASequence) {
  ASSERT_THAT(
      DOUBLE(pimc->standardEvaluator({{0}})->evaluateSequence(0, 1)),
      DoubleEq(0.5));
  ASSERT_THAT(
      DOUBLE(pimc->standardEvaluator({{1}})->evaluateSequence(0, 1)),
      DoubleEq(0.5));
  ASSERT_THAT(
      DOUBLE(pimc->standardEvaluator({{0, 1}})->evaluateSequence(0, 2)),
      DoubleEq(0.5 * 0.9));
  ASSERT_THAT(
      DOUBLE(pimc->standardEvaluator({{0, 0}})->evaluateSequence(0, 2)),
      DoubleEq(0.5 * 0.1));
  ASSERT_THAT(
      DOUBLE(pimc->standardEvaluator({{1, 0}})->evaluateSequence(0, 2)),
      DoubleEq(0.5 * 0.5));
  ASSERT_THAT(
      DOUBLE(pimc->standardEvaluator({{1, 1}})->evaluateSequence(0, 2)),
      DoubleEq(0.5 * 0.5));
  ASSERT_THAT(
      DOUBLE(pimc->standardEvaluator({{1, 0, 1}})->evaluateSequence(0, 3)),
      DoubleEq(0.5 * 0.5 * 0.8));
}

/*----------------------------------------------------------------------------*/

TEST_F(APeriodicIMC, ShouldEvaluateASequenceWithPrefixSumArray) {
  for (int i = 1; i < 1000; i++) {
    auto data = generateRandomSequence(i, 2);
    ASSERT_THAT(DOUBLE(pimc->standardEvaluator({ data }, true)
                           ->evaluateSequence(0, data.size())),
                DoubleEq(DOUBLE(pimc->standardEvaluator({ data })
                                    ->evaluateSequence(0, data.size()))));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APeriodicIMC, ShouldChooseSequenceWithDefaultSeed) {
  // TODO(igorbonadio): check bigger sequence
  ASSERT_THAT(pimc->standardGenerator()->drawSequence(5)[0],
              ContainerEq(Sequence{0, 1, 1, 0, 1}));
}

/*----------------------------------------------------------------------------*/
