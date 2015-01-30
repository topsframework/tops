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

using ::testing::Eq;
using ::testing::DoubleEq;

using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::Sequence;

using tops::helper::createMachlerVLMC;

class AVLMC : public testing::Test {
 protected:
  VariableLengthMarkovChainPtr vlmc = createMachlerVLMC();
};

TEST_F(AVLMC, ShouldEvaluateAPosition) {
  ASSERT_THAT(vlmc->evaluatePosition({0}, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->evaluatePosition({1}, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->evaluatePosition({0, 1}, 1), DoubleEq(log(0.40)));
  ASSERT_THAT(vlmc->evaluatePosition({0, 0}, 1), DoubleEq(log(0.60)));
  ASSERT_THAT(vlmc->evaluatePosition({1, 0}, 1), DoubleEq(log(0.30)));
  ASSERT_THAT(vlmc->evaluatePosition({1, 1}, 1), DoubleEq(log(0.70)));
  ASSERT_THAT(vlmc->evaluatePosition({1, 0, 1}, 2), DoubleEq(log(0.40)));
}

TEST_F(AVLMC, ShouldEvaluateASequence) {
  ASSERT_THAT(vlmc->evaluate({0}, 0, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->evaluate({1}, 0, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(vlmc->evaluate({0, 1}, 0, 1), DoubleEq(log(0.20)));
  ASSERT_THAT(vlmc->evaluate({0, 0}, 0, 1), DoubleEq(log(0.30)));
  ASSERT_THAT(vlmc->evaluate({1, 0}, 0, 1), DoubleEq(log(0.15)));
  ASSERT_THAT(vlmc->evaluate({1, 1}, 0, 1), DoubleEq(log(0.35)));
  ASSERT_THAT(vlmc->evaluate({1, 0, 1}, 0, 2), DoubleEq(log(0.06)));
}
