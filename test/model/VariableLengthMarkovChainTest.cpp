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

#include "model/VariableLengthMarkovChain.hpp"
#include "model/Sequence.hpp"
#include "model/DiscreteIIDModel.hpp"

#include "helper/VariableLengthMarkovChain.hpp"
#include "helper/Sequence.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;

using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::Sequence;

using tops::helper::createMachlerVLMC;
using tops::helper::generateRandomSequence;

class AVLMC : public testing::Test {
 protected:
  VariableLengthMarkovChainPtr vlmc = createMachlerVLMC();
};

TEST_F(AVLMC, ShouldHaveAnAlphabet) {
  ASSERT_THAT(vlmc->alphabetSize(), Eq(2));
}

TEST_F(AVLMC, ShouldEvaluateAPosition) {
  ASSERT_THAT(vlmc->evaluatePosition({0}, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->evaluatePosition({1}, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->evaluatePosition({0, 1}, 1), DoubleEq(log(0.80)));
  ASSERT_THAT(vlmc->evaluatePosition({0, 0}, 1), DoubleEq(log(0.20)));
  ASSERT_THAT(vlmc->evaluatePosition({1, 0}, 1), DoubleEq(log(0.21)));
  ASSERT_THAT(vlmc->evaluatePosition({1, 1}, 1), DoubleEq(log(0.79)));
  ASSERT_THAT(vlmc->evaluatePosition({1, 0, 1}, 2), DoubleEq(log(0.80)));
  ASSERT_THAT(vlmc->evaluatePosition({1, 0, 1, 0}, 3), DoubleEq(log(0.10)));
}

TEST_F(AVLMC, ShouldEvaluateASequence) {
  ASSERT_THAT(vlmc->evaluateSequence({0}, 0, 1), DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->evaluateSequence({1}, 0, 1), DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->evaluateSequence({0, 1}, 0, 2), DoubleEq(log(0.50) + log(0.80)));
  ASSERT_THAT(vlmc->evaluateSequence({0, 0}, 0, 2), DoubleEq(log(0.50) + log(0.20)));
  ASSERT_THAT(vlmc->evaluateSequence({1, 0}, 0, 2), DoubleEq(log(0.50) + log(0.21)));
  ASSERT_THAT(vlmc->evaluateSequence({1, 1}, 0, 2), DoubleEq(log(0.50) + log(0.79)));
  ASSERT_THAT(vlmc->evaluateSequence({1, 0, 1}, 0, 3), DoubleEq(log(0.50) + log(0.21) + log(0.80)));
  ASSERT_THAT(vlmc->evaluateSequence({1, 0, 1, 0}, 0, 4), DoubleEq(log(0.50) + log(0.21) + log(0.80) + log(0.10)));
}

TEST_F(AVLMC, ShouldEvaluateASequenceWithPrefixSumArray) {
  for (int i = 1; i < 1000; i++) {
    auto data = generateRandomSequence(i, 2);
    vlmc->initializePrefixSumArray(data);
    ASSERT_THAT(vlmc->evaluateWithPrefixSumArray(0, data.size()), DoubleEq(vlmc->evaluateSequence(data, 0, data.size())));
  }
}
