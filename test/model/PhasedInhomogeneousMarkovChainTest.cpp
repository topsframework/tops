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
#include "model/PhasedInhomogeneousMarkovChain.hpp"
#include "model/VariableLengthMarkovChain.hpp"
#include "model/ProbabilisticModelDecorator.hpp"
#include "model/Sequence.hpp"

#include "helper/VariableLengthMarkovChain.hpp"
#include "helper/Sequence.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;

using tops::model::Sequence;
using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::PhasedInhomogeneousMarkovChain;
using tops::model::PhasedInhomogeneousMarkovChainPtr;
using tops::model::ProbabilisticModelDecorator;
using tops::model::ProbabilisticModelDecoratorPtr;
using tops::model::ProbabilisticModelPtr;

using tops::helper::createMachlerVLMC;
using tops::helper::createVLMCMC;
using tops::helper::generateRandomSequence;

class APhasedInhomogeneousMarkovChain : public testing::Test {
 protected:
  PhasedInhomogeneousMarkovChainPtr imc;

  virtual void SetUp() {
    imc = PhasedInhomogeneousMarkovChain::make({createMachlerVLMC(),
                                                createVLMCMC()});
  }
};

TEST_F(APhasedInhomogeneousMarkovChain, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(imc->alphabetSize(), Eq(2));
}

TEST_F(APhasedInhomogeneousMarkovChain, ShouldEvaluateASequence) {
  ASSERT_THAT(imc->evaluateSequence({0}, 0, 1),
              DoubleEq(log(0.50)));
  ASSERT_THAT(imc->evaluateSequence({1}, 0, 1),
              DoubleEq(log(0.50)));
  ASSERT_THAT(imc->evaluateSequence({0, 1}, 0, 2),
              DoubleEq(log(0.50) + log(0.90)));
  ASSERT_THAT(imc->evaluateSequence({0, 0}, 0, 2),
              DoubleEq(log(0.50) + log(0.10)));
  ASSERT_THAT(imc->evaluateSequence({1, 0}, 0, 2),
              DoubleEq(log(0.50) + log(0.50)));
  ASSERT_THAT(imc->evaluateSequence({1, 1}, 0, 2),
              DoubleEq(log(0.50) + log(0.50)));
  ASSERT_THAT(imc->evaluateSequence({1, 0, 1}, 0, 3),
              DoubleEq(log(0.5) + log(0.5) + log(0.80)));
}

TEST_F(APhasedInhomogeneousMarkovChain,
       ShouldEvaluateASequenceWithPrefixSumArray) {
  for (int i = 1; i < 1000; i++) {
    auto data = generateRandomSequence(i, 2);
    imc->initializePrefixSumArray(data);
    ASSERT_THAT(imc->evaluateWithPrefixSumArray(0, data.size()),
                DoubleEq(imc->evaluateSequence(data, 0, data.size())));
  }
}

TEST_F(APhasedInhomogeneousMarkovChain, CanBeDecorated) {
  auto decorated_imc = ProbabilisticModelDecorator::make(imc);
  ASSERT_THAT(decorated_imc->evaluateSequence({0}, 0, 1),
              DoubleEq(log(0.50)));
  ASSERT_THAT(decorated_imc->evaluateSequence({1}, 0, 1),
              DoubleEq(log(0.50)));
  ASSERT_THAT(decorated_imc->evaluateSequence({0, 1}, 0, 2),
              DoubleEq(log(0.50) + log(0.90)));
  ASSERT_THAT(decorated_imc->evaluateSequence({0, 0}, 0, 2),
              DoubleEq(log(0.50) + log(0.10)));
  ASSERT_THAT(decorated_imc->evaluateSequence({1, 0}, 0, 2),
              DoubleEq(log(0.50) + log(0.50)));
  ASSERT_THAT(decorated_imc->evaluateSequence({1, 1}, 0, 2),
              DoubleEq(log(0.50) + log(0.50)));
  ASSERT_THAT(decorated_imc->evaluateSequence({1, 0, 1}, 0, 3),
              DoubleEq(log(0.5) + log(0.5) + log(0.80)));
}

TEST(PhasedInhomogeneousMarkovChain, ShouldBeTrained) {
  std::vector<Sequence> training_set = {{1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                                        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                        {1, 1, 0, 1, 0, 1, 1, 0, 1, 0},
                                        {0, 1, 1, 0, 0, 0, 0, 1, 0, 1}};
  auto imc
    = PhasedInhomogeneousMarkovChain::trainInterpolatedPhasedMarkovChain(
      training_set, {}, {}, 2, 2, 2, 1.5, {1.0, 1.0, 1.0, 1.0}, ProbabilisticModelPtr(NULL));
  // ASSERT_THAT(imc->alphabetSize(), Eq(2));
  // ASSERT_THAT(imc->evaluateSequence({1, 0, 1, 0}, 0, 4),
  //             DoubleNear(-2.99504, 1e-4));
  // ASSERT_THAT(imc->evaluateSequence({1, 1, 1, 1}, 0, 4),
  //             DoubleNear(-2.99504, 1e-4));
  // ASSERT_THAT(imc->evaluateSequence({0, 0, 0, 1, 1, 1, 1}, 0, 7),
  //             DoubleNear(-4.87431, 1e-4));
  // model_name = "InhomogeneousMarkovChain"
  // p0 = ("0" | "" : 0.552632; # leaf
  // "1" | "" : 0.447368; # leaf
  // )
  // p1 = ("0" | "" : 0.5; # leaf
  // "1" | "" : 0.5; # leaf
  // )
  // position_specific_distribution = ("p0","p1")
  // phased =1
  // alphabet = ("0", "1")
}
