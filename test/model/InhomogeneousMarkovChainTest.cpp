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
#include "model/Sequence.hpp"

#include "helper/VariableLengthMarkovChain.hpp"
#include "helper/Sequence.hpp"

// ToPS templates
#include "model/ProbabilisticModelDecorator.tcc"

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::ContainerEq;

using tops::model::Sequence;
using tops::model::INVALID_SYMBOL;
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
    imc = InhomogeneousMarkovChain::make(
      std::vector<VariableLengthMarkovChainPtr>{
        createMachlerVLMC(), createVLMCMC()
      }
    );
  }
};

TEST_F(AnInhomogeneousMarkovChain, ShouldEvaluateASequence) {
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
              DoubleEq(-std::numeric_limits<double>::infinity()));
}

TEST_F(AnInhomogeneousMarkovChain, ShouldEvaluateASequenceWithPrefixSumArray) {
  for (int i = 1; i < 1000; i++) {
    auto data = generateRandomSequence(i, 2);
    ASSERT_THAT(
        imc->standardEvaluator(data, true)->evaluateSequence(0, data.size()),
        DoubleEq(imc->standardEvaluator(data)->evaluateSequence(0, data.size())));
  }
}

TEST_F(AnInhomogeneousMarkovChain, CanBeDecorated) {
  auto decorated_imc
    = std::make_shared<ProbabilisticModelDecorator<InhomogeneousMarkovChain>>(imc);
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
  ASSERT_THAT(decorated_imc->standardEvaluator({1, 0, 1})->evaluateSequence(0, 3),
              DoubleEq(-std::numeric_limits<double>::infinity()));
}

TEST_F(AnInhomogeneousMarkovChain, ShouldChooseSequenceWithSeed42) {
  ASSERT_THAT(imc->standardGenerator()->drawSequence(5), ContainerEq(Sequence{1, 0, INVALID_SYMBOL, INVALID_SYMBOL, INVALID_SYMBOL}));
}
