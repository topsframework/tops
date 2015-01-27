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

#include "VariableLengthMarkovChain.hpp"
#include "Sequence.hpp"
#include "DiscreteIIDModel.hpp"

using ::testing::Eq;
using ::testing::DoubleEq;

using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::Sequence;

class AVLMC : public testing::Test {
 protected:
  tops::ContextTreePtr context_tree;
  VariableLengthMarkovChainPtr vlmc;

  virtual void SetUp() {
    auto alphabet = tops::AlphabetPtr(new tops::Alphabet());
    alphabet->createSymbol("0");
    alphabet->createSymbol("1");
    context_tree = tops::ContextTreePtr(new tops::ContextTree(alphabet));
    vlmc = VariableLengthMarkovChain::make(context_tree);
  }
};

TEST_F(AVLMC, ShouldHaveAnContextTree) {
  ASSERT_THAT(vlmc->getTree(), Eq(context_tree));
}
