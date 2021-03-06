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

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/Sequence.hpp"
#include "model/VariableLengthMarkovChain.hpp"

#include "exception/OutOfRange.hpp"

#include "helper/Sequence.hpp"
#include "helper/VariableLengthMarkovChain.hpp"

// Tested header
#include "model/InhomogeneousMarkovChain.hpp"

// Macros
#define DOUBLE(X) static_cast<double>(X)

/*----------------------------------------------------------------------------*/
/*                             USING DECLARATIONS                             */
/*----------------------------------------------------------------------------*/

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::ContainerEq;

using tops::model::Sequence;
using tops::model::INVALID_SYMBOL;
using tops::model::InhomogeneousMarkovChain;
using tops::model::VariableLengthMarkovChain;
using tops::model::InhomogeneousMarkovChainPtr;
using tops::model::VariableLengthMarkovChainPtr;

using tops::exception::OutOfRange;

using tops::helper::createMachlerVLMC;
using tops::helper::createVLMCMC;
using tops::helper::generateRandomSequence;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class AnInhomogeneousMarkovChain : public testing::Test {
 protected:
  InhomogeneousMarkovChainPtr imc;

  virtual void SetUp() {
    imc = InhomogeneousMarkovChain::make(
      std::vector<VariableLengthMarkovChainPtr>{
        createMachlerVLMC(), createVLMCMC() });
  }
};

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(AnInhomogeneousMarkovChain, ShouldEvaluateASequence) {
  ASSERT_THAT(DOUBLE(imc->standardEvaluator({0})->evaluateSequence(0, 1)),
              DoubleEq(0.50));
  ASSERT_THAT(DOUBLE(imc->standardEvaluator({1})->evaluateSequence(0, 1)),
              DoubleEq(0.50));
  ASSERT_THAT(DOUBLE(imc->standardEvaluator({0, 1})->evaluateSequence(0, 2)),
              DoubleEq(0.50 * 0.90));
  ASSERT_THAT(DOUBLE(imc->standardEvaluator({0, 0})->evaluateSequence(0, 2)),
              DoubleEq(0.50 * 0.10));
  ASSERT_THAT(DOUBLE(imc->standardEvaluator({1, 0})->evaluateSequence(0, 2)),
              DoubleEq(0.50 * 0.50));
  ASSERT_THAT(DOUBLE(imc->standardEvaluator({1, 1})->evaluateSequence(0, 2)),
              DoubleEq(0.50 * 0.50));
  ASSERT_THAT(DOUBLE(imc->standardEvaluator({1, 0, 1})->evaluateSequence(0, 3)),
              DoubleEq(0));
}

/*----------------------------------------------------------------------------*/

TEST_F(AnInhomogeneousMarkovChain, ShouldEvaluateASequenceWithPrefixSumArray) {
  for (int i = 1; i < 1000; i++) {
    auto data = generateRandomSequence(i, 2);
    auto size = data.size();
    ASSERT_THAT(
        DOUBLE(imc->standardEvaluator(data, true)->evaluateSequence(0, size)),
        DoubleEq(imc->standardEvaluator(data)->evaluateSequence(0, size)));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(AnInhomogeneousMarkovChain, ShouldChooseSequenceWithDefaultSeed) {
  ASSERT_THAT(imc->standardGenerator()->drawSequence(2),
              ContainerEq(Sequence{ 0, 1 }));
}

/*----------------------------------------------------------------------------*/

TEST_F(AnInhomogeneousMarkovChain, ShouldThrowAnOutOfRangeInDrawSymbol) {
  ASSERT_THROW(imc->standardGenerator()->drawSequence(3), OutOfRange);
}

/*----------------------------------------------------------------------------*/
