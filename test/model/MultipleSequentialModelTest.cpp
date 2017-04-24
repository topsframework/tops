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

#include "helper/DiscreteIIDModel.hpp"
#include "helper/VariableLengthMarkovChain.hpp"

// Tested header
#include "model/MultipleSequentialModel.hpp"

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
using tops::model::MultipleSequentialModel;
using tops::model::MultipleSequentialModelPtr;

using tops::helper::createMachlerVLMC;
using tops::helper::createLoadedCoinIIDModel;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class AMultipleSequentialModel : public testing::Test {
 protected:
  MultipleSequentialModelPtr mm = MultipleSequentialModel::make(
    std::vector<ProbabilisticModelPtr>{
      createLoadedCoinIIDModel(), createMachlerVLMC() },
    std::vector<int>{3, 4});
};

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(AMultipleSequentialModel, ShouldEvaluateASequence) {
  ASSERT_THAT(
      DOUBLE(mm->standardEvaluator({1, 0, 1, 0, 0, 0, 0})
               ->evaluateSequence(0, 7)),
      DoubleNear(0.0001024, 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST_F(AMultipleSequentialModel, ShouldEvaluateASequenceWithPrefixSumArray) {
  ASSERT_THAT(DOUBLE(mm->standardEvaluator({1, 0, 1, 0, 0, 0, 0}, true)
                       ->evaluateSequence(0, 7)),
              DoubleNear(DOUBLE(mm->standardEvaluator({1, 0, 1, 0, 0, 0, 0})
                                  ->evaluateSequence(0, 7)), 1e-4));
}

/*----------------------------------------------------------------------------*/

TEST_F(AMultipleSequentialModel, ShouldChooseSequenceWithDefaultSeed) {
  // TODO(igorbonadio): check bigger sequence
  ASSERT_THAT(mm->standardGenerator()->drawSequence(5),
              ContainerEq(Sequence{0, 1, 1, 0, 1}));
}

/*----------------------------------------------------------------------------*/
