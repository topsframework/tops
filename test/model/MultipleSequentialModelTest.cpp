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
#include "model/MultipleSequentialModel.hpp"
#include "model/Sequence.hpp"

#include "helper/DiscreteIIDModel.hpp"
#include "helper/VariableLengthMarkovChain.hpp"

using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::Eq;

using tops::model::MultipleSequentialModel;
using tops::model::MultipleSequentialModelPtr;
using tops::model::Sequence;

using tops::helper::createLoadedCoinIIDModel;
using tops::helper::createMachlerVLMC;

class AMultipleSequentialModel : public testing::Test {
 protected:
  MultipleSequentialModelPtr mm = MultipleSequentialModel::make(
    {createLoadedCoinIIDModel(), createMachlerVLMC()},
    {3, 4});
};

TEST_F(AMultipleSequentialModel, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(mm->alphabetSize(), Eq(2));
}

TEST_F(AMultipleSequentialModel, ShouldEvaluateASequence) {
  ASSERT_THAT(mm->evaluateSequence({1, 0, 1}, 0, 3),
              DoubleNear(-1.83258, 1e-4));
  ASSERT_THAT(mm->evaluateSequence({1, 0, 1, 0}, 0, 4),
              DoubleNear(-4.13517, 1e-4));
  ASSERT_THAT(mm->evaluateSequence({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 13),
              DoubleNear(-18.6201, 1e-4));
}

TEST_F(AMultipleSequentialModel, ShouldEvaluateASequenceWithPrefixSumArray) {
  mm->initializePrefixSumArray({1, 0, 1});
  ASSERT_THAT(mm->evaluateWithPrefixSumArray(0, 3),
              DoubleNear(mm->evaluateSequence({1, 0, 1}, 0, 3), 1e-4));

  mm->initializePrefixSumArray({1, 0, 1, 0});
  ASSERT_THAT(mm->evaluateWithPrefixSumArray(0, 4),
              DoubleNear(mm->evaluateSequence({1, 0, 1, 0}, 0, 4), 1e-4));

  mm->initializePrefixSumArray({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
  ASSERT_THAT(mm->evaluateWithPrefixSumArray(0, 13),
              DoubleNear(mm->evaluateSequence({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 13), 1e-4));
}