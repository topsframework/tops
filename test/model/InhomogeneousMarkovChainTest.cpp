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
#include "model/InhomogeneousMarkovChain.hpp"
#include "model/VariableLengthMarkovChain.hpp"
#include "model/ProbabilisticModelDecorator.hpp"
#include "model/Sequence.hpp"

#include "helper/VariableLengthMarkovChain.hpp"
#include "helper/Sequence.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;

using tops::model::Sequence;
using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::InhomogeneousMarkovChain;
using tops::model::InhomogeneousMarkovChainPtr;
using tops::model::ProbabilisticModelDecorator;
using tops::model::ProbabilisticModelDecoratorPtr;

using tops::helper::createMachlerVLMC;
using tops::helper::createVLMCMC;
using tops::helper::generateRandomSequence;

class AnInhomogeneousMarkovChain : public testing::Test {
 protected:
  InhomogeneousMarkovChainPtr imc;

  virtual void SetUp() {
    imc = InhomogeneousMarkovChain::make({createMachlerVLMC(), createVLMCMC()});
  }
};

TEST_F(AnInhomogeneousMarkovChain, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(imc->alphabetSize(), Eq(2));
}

TEST_F(AnInhomogeneousMarkovChain, ShouldEvaluateASequence) {
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
              DoubleEq(-HUGE));
}

TEST_F(AnInhomogeneousMarkovChain, ShouldEvaluateASequenceWithPrefixSumArray) {
  for (int i = 1; i < 1000; i++) {
    auto data = generateRandomSequence(i, 2);
    imc->initializePrefixSumArray(data);
    ASSERT_THAT(
        imc->evaluateWithPrefixSumArray(0, data.size()),
        DoubleEq(imc->evaluateSequence(data, 0, data.size())));
  }
}

TEST_F(AnInhomogeneousMarkovChain, CanBeDecorated) {
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
              DoubleEq(-HUGE));
}
