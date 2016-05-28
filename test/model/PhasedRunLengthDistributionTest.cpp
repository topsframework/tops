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

#include "helper/Sequence.hpp"

// Tested header
#include "model/PhasedRunLengthDistribution.hpp"
#include "helper/PhasedRunLengthDistribution.hpp"

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
using tops::model::PhasedRunLengthDistribution;
using tops::model::PhasedRunLengthDistributionPtr;

using tops::helper::sequenceOfLengths;
using tops::helper::generateRandomSequence;
using tops::helper::createSamplePhasedRunLengthDistribution;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class APhasedRunLengthDistribution : public testing::Test {
 protected:
  PhasedRunLengthDistributionPtr distribution
    = createSamplePhasedRunLengthDistribution();
};

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(APhasedRunLengthDistribution, ShouldHaveAnAlphabetSize) {
  ASSERT_THAT(distribution->alphabetSize(), Eq(15000));
}

/*----------------------------------------------------------------------------*/

TEST_F(APhasedRunLengthDistribution, ShouldEvaluateASingleSymbol) {
  ASSERT_THAT(DOUBLE(distribution->probabilityOf(125)),
              DoubleNear(0.008310, 1e-04));
  ASSERT_THAT(DOUBLE(distribution->probabilityOf(4187)),
              DoubleNear(0.000034, 1e-04));
  ASSERT_THAT(DOUBLE(distribution->probabilityOf(4188)),
              DoubleEq(0));
}

/*----------------------------------------------------------------------------*/

TEST_F(APhasedRunLengthDistribution, ShouldDrawSequenceWithDefaultSeed) {
  // TODO(igorbonadio): check bigger sequence
  ASSERT_THAT(distribution->standardGenerator()->drawSequence(5),
              ContainerEq(Sequence{5, 2015, 11885, 23, 44}));
}

/*----------------------------------------------------------------------------*/
