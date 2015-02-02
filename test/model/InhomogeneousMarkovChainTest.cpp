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

#include "model/InhomogeneousMarkovChain.hpp"
#include "model/VariableLengthMarkovChain.hpp"
#include "model/Sequence.hpp"

#include "helper/VariableLengthMarkovChain.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;

using tops::model::Sequence;
using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::InhomogeneousMarkovChain;
using tops::model::InhomogeneousMarkovChainPtr;

using tops::helper::createMachlerVLMC;
using tops::helper::createVLMCMC;

class AInhomogeneousMarkovChain : public testing::Test {
 protected:
  InhomogeneousMarkovChainPtr imc;

  virtual void SetUp() {
    imc = InhomogeneousMarkovChain::make({createMachlerVLMC(), createVLMCMC()}, true);
  }
};

TEST_F(AInhomogeneousMarkovChain, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(imc->alphabetSize(), Eq(2));
}

TEST_F(AInhomogeneousMarkovChain, ShouldEvaluateASequence) {
  ASSERT_THAT(imc->evaluateSequence({0}, 0, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(imc->evaluateSequence({1}, 0, 0), DoubleEq(log(0.50)));
  ASSERT_THAT(imc->evaluateSequence({0, 1}, 0, 1), DoubleEq(log(0.50) + log(0.90)));
  ASSERT_THAT(imc->evaluateSequence({0, 0}, 0, 1), DoubleEq(log(0.50) + log(0.10)));
  ASSERT_THAT(imc->evaluateSequence({1, 0}, 0, 1), DoubleEq(log(0.50) + log(0.50)));
  ASSERT_THAT(imc->evaluateSequence({1, 1}, 0, 1), DoubleEq(log(0.50) + log(0.50)));
  ASSERT_THAT(imc->evaluateSequence({1, 0, 1}, 0, 2), DoubleEq(log(0.5) + log(0.5) + log(0.80)));
}
