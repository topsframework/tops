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
#include <iomanip>

// External headers
#include "gmock/gmock.h"

// ToPS headers
#include "model/Util.hpp"
#include "model/Sequence.hpp"
#include "model/Probability.hpp"
#include "model/RandomNumberGeneratorAdapter.hpp"

#include "exception/NotYetImplemented.hpp"

#include "helper/Sequence.hpp"
#include "helper/SExprTranslator.hpp"

// Tested header
#include "model/ContextSensitiveHiddenMarkovModel.hpp"
#include "helper/ContextSensitiveHiddenMarkovModel.hpp"

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
using tops::model::Sequences;
using tops::model::RandomNumberGeneratorAdapter;
using tops::model::ContextSensitiveHiddenMarkovModel;
using tops::model::ContextSensitiveHiddenMarkovModelPtr;

using tops::exception::NotYetImplemented;

using tops::helper::createDishonestCoinCasinoCSHMM;
using tops::helper::createUntrainedDishonestCoinCasinoCSHMM;

using CSHMM = tops::model::ContextSensitiveHiddenMarkovModel;
using CSHMMPtr = tops::model::ContextSensitiveHiddenMarkovModelPtr;

/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/

class ACSHMM : public testing::Test {
 protected:
  CSHMMPtr cshmm = createDishonestCoinCasinoCSHMM();
};

/*----------------------------------------------------------------------------*/
/*                                SIMPLE TESTS                                */
/*----------------------------------------------------------------------------*/

TEST(CSHMM, ShouldBeTrainedUsingBaumWelchAlgorithm) {
  auto cshmm_trainer = CSHMM::unsupervisedTrainer();

  cshmm_trainer->add_training_set({
    // Sequences with size 4 to 6 generated with default seed
    // Empty spaces indicate gaps in the sequence

    { { 1, 1, 1, 1, } },  // { 0, 1, 1, 1, 2, 3, }
    { { 1, 0, 1, 1, } },  // { 0, 1, 1, 1, 2, 3, }
    { { 1, 1, 1, 1, } },  // { 0, 2, 2, 2, 2, 3, }
    { { 0, 1, 0, 1, } },  // { 0, 1, 2, 2, 1, 3, }
    { { 1, 0, 0, 0, } },  // { 0, 2, 1, 1, 1, 3, }
    { { 1, 0, 1, 1, } },  // { 0, 2, 2, 2, 2, 3, }
    { { 1, 0, 1, 1, } },  // { 0, 2, 2, 2, 2, 3, }
    { { 1, 1, 0, 1, } },  // { 0, 1, 1, 1, 1, 3, }
    { { 1, 1, 1, 1, } },  // { 0, 2, 2, 1, 1, 3, }
    { { 1, 1, 1, 1, } },  // { 0, 2, 2, 2, 2, 3, }
    { { 1, 1, 1, 1, } },  // { 0, 2, 2, 2, 2, 3, }
    { { 0, 1, 1, 1, } },  // { 0, 1, 1, 2, 2, 3, }
    { { 1, 1, 1, 1, } },  // { 0, 2, 2, 2, 2, 3, }
    { { 1, 1, 1, 1, } },  // { 0, 2, 1, 1, 1, 3, }
    { { 1, 0, 1, 1, } },  // { 0, 1, 2, 2, 1, 3, }
    { { 1, 0, 0, 1, } },  // { 0, 1, 1, 1, 1, 3, }
    { { 0, 1, 0, 1, } },  // { 0, 1, 1, 1, 1, 3, }
    { { 1, 0, 1, 1, } },  // { 0, 2, 1, 2, 2, 3, }
    { { 0, 1, 0, 1, } },  // { 0, 1, 2, 2, 2, 3, }
    { { 0, 0, 1, 1, } },  // { 0, 1, 1, 2, 2, 3, }
    { { 0, 1, 0, 0, } },  // { 0, 1, 1, 1, 1, 3, }
    { { 0, 1, 1, 0, } },  // { 0, 2, 2, 2, 2, 3, }
    { { 1, 0, 1, 0, } },  // { 0, 1, 1, 2, 2, 3, }
    { { 1, 0, 1, 1, } },  // { 0, 2, 1, 2, 1, 3, }
    { { 1, 1, 1, 0, } },  // { 0, 2, 2, 2, 1, 3, }
    { { 0, 0, 1, 1, } },  // { 0, 1, 2, 2, 2, 3, }
    { { 0, 1, 0, 1, } },  // { 0, 2, 2, 2, 2, 3, }
    { { 1, 0, 0, 1, } },  // { 0, 2, 1, 1, 1, 3, }
    { { 1, 1, 1, 1, } },  // { 0, 1, 2, 1, 2, 3, }
    { { 0, 1, 0, 0, } },  // { 0, 1, 2, 2, 1, 3, }
    { { 0, 1, 1, 1, } },  // { 0, 1, 1, 1, 1, 3, }
    { { 0, 1, 1, 1, } },  // { 0, 1, 1, 1, 1, 3, }

    { { 0, 1, 1, 1, 0, } },  // { 0, 1, 1, 1, 1, 1, 3, }
    { { 1, 1, 0, 0, 1, } },  // { 0, 2, 2, 1, 2, 2, 3, }
    { { 1, 0, 1, 1, 0, } },  // { 0, 1, 1, 1, 2, 1, 3, }
    { { 1, 1, 1, 1, 0, } },  // { 0, 1, 1, 2, 2, 1, 3, }
    { { 1, 1, 1, 0, 1, } },  // { 0, 1, 1, 2, 2, 2, 3, }
    { { 0, 1, 0, 1, 1, } },  // { 0, 1, 2, 2, 2, 2, 3, }
    { { 0, 0, 1, 0, 0, } },  // { 0, 1, 1, 1, 1, 1, 3, }
    { { 0, 1, 1, 1, 1, } },  // { 0, 1, 1, 2, 2, 1, 3, }
    { { 0, 1, 1, 1, 1, } },  // { 0, 2, 2, 2, 2, 2, 3, }
    { { 1, 0, 1, 1, 1, } },  // { 0, 1, 1, 2, 2, 2, 3, }
    { { 0, 1, 1, 1, 1, } },  // { 0, 2, 2, 2, 2, 2, 3, }
    { { 0, 1, 0, 1, 0, } },  // { 0, 1, 2, 2, 2, 1, 3, }
    { { 1, 0, 0, 1, 0, } },  // { 0, 2, 2, 1, 1, 1, 3, }
    { { 1, 1, 1, 0, 0, } },  // { 0, 1, 1, 1, 1, 1, 3, }
    { { 1, 1, 0, 1, 1, } },  // { 0, 2, 2, 2, 2, 2, 3, }
    { { 1, 0, 1, 1, 1, } },  // { 0, 1, 1, 1, 2, 2, 3, }
    { { 1, 0, 1, 1, 0, } },  // { 0, 1, 2, 2, 1, 2, 3, }
    { { 1, 0, 1, 1, 0, } },  // { 0, 2, 2, 1, 1, 2, 3, }
    { { 0, 0, 0, 1, 1, } },  // { 0, 1, 1, 1, 2, 2, 3, }
    { { 1, 1, 0, 0, 1, } },  // { 0, 1, 2, 2, 2, 2, 3, }
    { { 1, 1, 1, 0, 0, } },  // { 0, 2, 2, 2, 1, 1, 3, }
    { { 1, 1, 1, 0, 0, } },  // { 0, 2, 2, 2, 2, 2, 3, }
    { { 1, 1, 1, 1, 0, } },  // { 0, 1, 2, 2, 2, 1, 3, }
    { { 1, 1, 0, 0, 1, } },  // { 0, 2, 2, 2, 1, 1, 3, }
    { { 1, 1, 1, 0, 1, } },  // { 0, 2, 2, 1, 1, 1, 3, }
    { { 0, 1, 1, 0, 1, } },  // { 0, 1, 1, 1, 1, 1, 3, }
    { { 1, 0, 0, 0, 0, } },  // { 0, 2, 1, 1, 1, 1, 3, }
    { { 0, 1, 0, 0, 0, } },  // { 0, 1, 1, 1, 1, 1, 3, }
    { { 1, 0, 0, 1, 1, } },  // { 0, 2, 1, 1, 1, 2, 3, }
    { { 1, 0, 1, 1, 1, } },  // { 0, 2, 2, 2, 2, 1, 3, }
    { { 0, 0, 1, 1, 0, } },  // { 0, 1, 1, 1, 1, 1, 3, }
    { { 0, 1, 1, 0, 1, } },  // { 0, 1, 1, 1, 1, 2, 3, }

    { { 1, 1, 1, 1, 0, 1, } },  // { 0, 1, 2, 2, 1, 1, 1, 3, }
    { { 1, 1, 0, 0, 0, 0, } },  // { 0, 2, 1, 1, 1, 1, 1, 3, }
    { { 0, 1, 1, 0, 0, 1, } },  // { 0, 2, 2, 2, 2, 1, 2, 3, }
    { { 1, 0, 1, 0, 1, 0, } },  // { 0, 2, 2, 1, 1, 1, 1, 3, }
    { { 0, 1, 0, 1, 0, 1, } },  // { 0, 1, 2, 1, 2, 1, 2, 3, }
    { { 1, 0, 0, 1, 1, 0, } },  // { 0, 2, 2, 2, 2, 2, 1, 3, }
    { { 1, 1, 0, 0, 1, 1, } },  // { 0, 2, 2, 2, 1, 2, 2, 3, }
    { { 0, 1, 1, 0, 0, 1, } },  // { 0, 2, 2, 1, 1, 1, 2, 3, }
    { { 1, 0, 0, 1, 1, 1, } },  // { 0, 1, 1, 1, 2, 2, 2, 3, }
    { { 1, 1, 1, 1, 0, 1, } },  // { 0, 2, 2, 2, 2, 1, 1, 3, }
    { { 1, 1, 0, 1, 1, 1, } },  // { 0, 2, 2, 2, 2, 1, 2, 3, }
    { { 1, 1, 1, 1, 1, 1, } },  // { 0, 2, 2, 2, 2, 2, 1, 3, }
    { { 1, 1, 0, 0, 1, 1, } },  // { 0, 1, 1, 1, 1, 1, 1, 3, }
    { { 1, 1, 0, 0, 1, 0, } },  // { 0, 1, 2, 1, 1, 1, 2, 3, }
    { { 0, 1, 0, 0, 1, 0, } },  // { 0, 1, 1, 1, 1, 1, 1, 3, }
    { { 1, 1, 1, 1, 1, 1, } },  // { 0, 2, 2, 1, 2, 2, 2, 3, }
    { { 1, 1, 1, 0, 0, 1, } },  // { 0, 2, 2, 2, 1, 1, 2, 3, }
    { { 0, 0, 1, 0, 0, 0, } },  // { 0, 1, 1, 1, 1, 1, 1, 3, }
    { { 1, 1, 1, 1, 1, 0, } },  // { 0, 2, 1, 2, 2, 2, 1, 3, }
    { { 1, 1, 1, 1, 0, 1, } },  // { 0, 2, 2, 2, 2, 2, 2, 3, }
    { { 0, 1, 1, 1, 1, 1, } },  // { 0, 1, 1, 1, 2, 2, 2, 3, }
    { { 1, 1, 1, 1, 1, 1, } },  // { 0, 2, 2, 2, 1, 1, 1, 3, }
    { { 1, 0, 1, 0, 0, 0, } },  // { 0, 2, 2, 2, 2, 1, 1, 3, }
    { { 1, 0, 0, 1, 1, 0, } },  // { 0, 2, 2, 2, 2, 2, 2, 3, }
    { { 0, 1, 1, 1, 1, 1, } },  // { 0, 1, 2, 2, 2, 1, 1, 3, }
    { { 1, 1, 1, 0, 0, 1, } },  // { 0, 2, 2, 2, 2, 1, 1, 3, }
    { { 1, 1, 1, 1, 1, 0, } },  // { 0, 2, 2, 2, 2, 2, 1, 3, }
    { { 0, 1, 1, 0, 0, 0, } },  // { 0, 2, 2, 1, 1, 1, 1, 3, }
    { { 1, 1, 1, 0, 1, 0, } },  // { 0, 1, 1, 2, 2, 2, 2, 3, }
    { { 0, 1, 1, 1, 1, 1, } },  // { 0, 2, 2, 2, 2, 2, 2, 3, }
    { { 1, 1, 0, 0, 1, 1, } },  // { 0, 1, 2, 2, 2, 2, 2, 3, }
    { { 0, 1, 0, 1, 0, 0, } },  // { 0, 1, 1, 1, 1, 1, 1, 3, }
  });

  auto trained_cshmm = cshmm_trainer->train(
    CSHMM::baum_welch_algorithm{},
    createUntrainedDishonestCoinCasinoCSHMM(), 1000, 1e-7);

  auto translator = tops::helper::SExprTranslator::make();
  auto serializer = trained_cshmm->serializer(translator);
  serializer->serialize();

  ASSERT_THAT(translator->sexpr(),
    Eq("(ContextSensitiveHiddenMarkovModel: "
         "(State: "
           "(DiscreteIIDModel: 0.000000 0.000000 1.000000) "
           "(DiscreteIIDModel: 0.000000 0.333333 0.333333 0.333333) "
           "(FixedDuration: fixedLength = 1)) "
         "(State: "
           "(DiscreteIIDModel: 0.358333 0.641667 0.000000) "
           "(DiscreteIIDModel: 0.000000 0.400000 0.400000 0.200000) "
           "(FixedDuration: fixedLength = 1)) "
         "(State: "
           "(DiscreteIIDModel: 0.358333 0.641667 0.000000) "
           "(DiscreteIIDModel: 0.000000 0.400000 0.400000 0.200000) "
           "(FixedDuration: fixedLength = 1)) "
         "(State: "
           "(DiscreteIIDModel: 0.000000 0.000000 1.000000) "
           "(DiscreteIIDModel: 0.000000 0.000000 0.000000 1.000000) "
           "(FixedDuration: fixedLength = 1)))"));
}

TEST(CSHMM, ShouldBeTrainedUsingMaximumLikelihoodAlgorithm) {
  auto cshmm_trainer = CSHMM::supervisedTrainer();

  cshmm_trainer->add_training_set({
    // Sequences with size 4 to 6 generated with default seed
    // Empty spaces indicate gaps in the sequence

    { { 1, 1, 1, 1, }, {}, { 0, 1, 1, 1, 2, 3, } },
    { { 1, 0, 1, 1, }, {}, { 0, 1, 1, 1, 2, 3, } },
    { { 1, 1, 1, 1, }, {}, { 0, 2, 2, 2, 2, 3, } },
    { { 0, 1, 0, 1, }, {}, { 0, 1, 2, 2, 1, 3, } },
    { { 1, 0, 0, 0, }, {}, { 0, 2, 1, 1, 1, 3, } },
    { { 1, 0, 1, 1, }, {}, { 0, 2, 2, 2, 2, 3, } },
    { { 1, 0, 1, 1, }, {}, { 0, 2, 2, 2, 2, 3, } },
    { { 1, 1, 0, 1, }, {}, { 0, 1, 1, 1, 1, 3, } },
    { { 1, 1, 1, 1, }, {}, { 0, 2, 2, 1, 1, 3, } },
    { { 1, 1, 1, 1, }, {}, { 0, 2, 2, 2, 2, 3, } },
    { { 1, 1, 1, 1, }, {}, { 0, 2, 2, 2, 2, 3, } },
    { { 0, 1, 1, 1, }, {}, { 0, 1, 1, 2, 2, 3, } },
    { { 1, 1, 1, 1, }, {}, { 0, 2, 2, 2, 2, 3, } },
    { { 1, 1, 1, 1, }, {}, { 0, 2, 1, 1, 1, 3, } },
    { { 1, 0, 1, 1, }, {}, { 0, 1, 2, 2, 1, 3, } },
    { { 1, 0, 0, 1, }, {}, { 0, 1, 1, 1, 1, 3, } },
    { { 0, 1, 0, 1, }, {}, { 0, 1, 1, 1, 1, 3, } },
    { { 1, 0, 1, 1, }, {}, { 0, 2, 1, 2, 2, 3, } },
    { { 0, 1, 0, 1, }, {}, { 0, 1, 2, 2, 2, 3, } },
    { { 0, 0, 1, 1, }, {}, { 0, 1, 1, 2, 2, 3, } },
    { { 0, 1, 0, 0, }, {}, { 0, 1, 1, 1, 1, 3, } },
    { { 0, 1, 1, 0, }, {}, { 0, 2, 2, 2, 2, 3, } },
    { { 1, 0, 1, 0, }, {}, { 0, 1, 1, 2, 2, 3, } },
    { { 1, 0, 1, 1, }, {}, { 0, 2, 1, 2, 1, 3, } },
    { { 1, 1, 1, 0, }, {}, { 0, 2, 2, 2, 1, 3, } },
    { { 0, 0, 1, 1, }, {}, { 0, 1, 2, 2, 2, 3, } },
    { { 0, 1, 0, 1, }, {}, { 0, 2, 2, 2, 2, 3, } },
    { { 1, 0, 0, 1, }, {}, { 0, 2, 1, 1, 1, 3, } },
    { { 1, 1, 1, 1, }, {}, { 0, 1, 2, 1, 2, 3, } },
    { { 0, 1, 0, 0, }, {}, { 0, 1, 2, 2, 1, 3, } },
    { { 0, 1, 1, 1, }, {}, { 0, 1, 1, 1, 1, 3, } },
    { { 0, 1, 1, 1, }, {}, { 0, 1, 1, 1, 1, 3, } },

    { { 0, 1, 1, 1, 0, }, {}, { 0, 1, 1, 1, 1, 1, 3, } },
    { { 1, 1, 0, 0, 1, }, {}, { 0, 2, 2, 1, 2, 2, 3, } },
    { { 1, 0, 1, 1, 0, }, {}, { 0, 1, 1, 1, 2, 1, 3, } },
    { { 1, 1, 1, 1, 0, }, {}, { 0, 1, 1, 2, 2, 1, 3, } },
    { { 1, 1, 1, 0, 1, }, {}, { 0, 1, 1, 2, 2, 2, 3, } },
    { { 0, 1, 0, 1, 1, }, {}, { 0, 1, 2, 2, 2, 2, 3, } },
    { { 0, 0, 1, 0, 0, }, {}, { 0, 1, 1, 1, 1, 1, 3, } },
    { { 0, 1, 1, 1, 1, }, {}, { 0, 1, 1, 2, 2, 1, 3, } },
    { { 0, 1, 1, 1, 1, }, {}, { 0, 2, 2, 2, 2, 2, 3, } },
    { { 1, 0, 1, 1, 1, }, {}, { 0, 1, 1, 2, 2, 2, 3, } },
    { { 0, 1, 1, 1, 1, }, {}, { 0, 2, 2, 2, 2, 2, 3, } },
    { { 0, 1, 0, 1, 0, }, {}, { 0, 1, 2, 2, 2, 1, 3, } },
    { { 1, 0, 0, 1, 0, }, {}, { 0, 2, 2, 1, 1, 1, 3, } },
    { { 1, 1, 1, 0, 0, }, {}, { 0, 1, 1, 1, 1, 1, 3, } },
    { { 1, 1, 0, 1, 1, }, {}, { 0, 2, 2, 2, 2, 2, 3, } },
    { { 1, 0, 1, 1, 1, }, {}, { 0, 1, 1, 1, 2, 2, 3, } },
    { { 1, 0, 1, 1, 0, }, {}, { 0, 1, 2, 2, 1, 2, 3, } },
    { { 1, 0, 1, 1, 0, }, {}, { 0, 2, 2, 1, 1, 2, 3, } },
    { { 0, 0, 0, 1, 1, }, {}, { 0, 1, 1, 1, 2, 2, 3, } },
    { { 1, 1, 0, 0, 1, }, {}, { 0, 1, 2, 2, 2, 2, 3, } },
    { { 1, 1, 1, 0, 0, }, {}, { 0, 2, 2, 2, 1, 1, 3, } },
    { { 1, 1, 1, 0, 0, }, {}, { 0, 2, 2, 2, 2, 2, 3, } },
    { { 1, 1, 1, 1, 0, }, {}, { 0, 1, 2, 2, 2, 1, 3, } },
    { { 1, 1, 0, 0, 1, }, {}, { 0, 2, 2, 2, 1, 1, 3, } },
    { { 1, 1, 1, 0, 1, }, {}, { 0, 2, 2, 1, 1, 1, 3, } },
    { { 0, 1, 1, 0, 1, }, {}, { 0, 1, 1, 1, 1, 1, 3, } },
    { { 1, 0, 0, 0, 0, }, {}, { 0, 2, 1, 1, 1, 1, 3, } },
    { { 0, 1, 0, 0, 0, }, {}, { 0, 1, 1, 1, 1, 1, 3, } },
    { { 1, 0, 0, 1, 1, }, {}, { 0, 2, 1, 1, 1, 2, 3, } },
    { { 1, 0, 1, 1, 1, }, {}, { 0, 2, 2, 2, 2, 1, 3, } },
    { { 0, 0, 1, 1, 0, }, {}, { 0, 1, 1, 1, 1, 1, 3, } },
    { { 0, 1, 1, 0, 1, }, {}, { 0, 1, 1, 1, 1, 2, 3, } },

    { { 1, 1, 1, 1, 0, 1, }, {}, { 0, 1, 2, 2, 1, 1, 1, 3, } },
    { { 1, 1, 0, 0, 0, 0, }, {}, { 0, 2, 1, 1, 1, 1, 1, 3, } },
    { { 0, 1, 1, 0, 0, 1, }, {}, { 0, 2, 2, 2, 2, 1, 2, 3, } },
    { { 1, 0, 1, 0, 1, 0, }, {}, { 0, 2, 2, 1, 1, 1, 1, 3, } },
    { { 0, 1, 0, 1, 0, 1, }, {}, { 0, 1, 2, 1, 2, 1, 2, 3, } },
    { { 1, 0, 0, 1, 1, 0, }, {}, { 0, 2, 2, 2, 2, 2, 1, 3, } },
    { { 1, 1, 0, 0, 1, 1, }, {}, { 0, 2, 2, 2, 1, 2, 2, 3, } },
    { { 0, 1, 1, 0, 0, 1, }, {}, { 0, 2, 2, 1, 1, 1, 2, 3, } },
    { { 1, 0, 0, 1, 1, 1, }, {}, { 0, 1, 1, 1, 2, 2, 2, 3, } },
    { { 1, 1, 1, 1, 0, 1, }, {}, { 0, 2, 2, 2, 2, 1, 1, 3, } },
    { { 1, 1, 0, 1, 1, 1, }, {}, { 0, 2, 2, 2, 2, 1, 2, 3, } },
    { { 1, 1, 1, 1, 1, 1, }, {}, { 0, 2, 2, 2, 2, 2, 1, 3, } },
    { { 1, 1, 0, 0, 1, 1, }, {}, { 0, 1, 1, 1, 1, 1, 1, 3, } },
    { { 1, 1, 0, 0, 1, 0, }, {}, { 0, 1, 2, 1, 1, 1, 2, 3, } },
    { { 0, 1, 0, 0, 1, 0, }, {}, { 0, 1, 1, 1, 1, 1, 1, 3, } },
    { { 1, 1, 1, 1, 1, 1, }, {}, { 0, 2, 2, 1, 2, 2, 2, 3, } },
    { { 1, 1, 1, 0, 0, 1, }, {}, { 0, 2, 2, 2, 1, 1, 2, 3, } },
    { { 0, 0, 1, 0, 0, 0, }, {}, { 0, 1, 1, 1, 1, 1, 1, 3, } },
    { { 1, 1, 1, 1, 1, 0, }, {}, { 0, 2, 1, 2, 2, 2, 1, 3, } },
    { { 1, 1, 1, 1, 0, 1, }, {}, { 0, 2, 2, 2, 2, 2, 2, 3, } },
    { { 0, 1, 1, 1, 1, 1, }, {}, { 0, 1, 1, 1, 2, 2, 2, 3, } },
    { { 1, 1, 1, 1, 1, 1, }, {}, { 0, 2, 2, 2, 1, 1, 1, 3, } },
    { { 1, 0, 1, 0, 0, 0, }, {}, { 0, 2, 2, 2, 2, 1, 1, 3, } },
    { { 1, 0, 0, 1, 1, 0, }, {}, { 0, 2, 2, 2, 2, 2, 2, 3, } },
    { { 0, 1, 1, 1, 1, 1, }, {}, { 0, 1, 2, 2, 2, 1, 1, 3, } },
    { { 1, 1, 1, 0, 0, 1, }, {}, { 0, 2, 2, 2, 2, 1, 1, 3, } },
    { { 1, 1, 1, 1, 1, 0, }, {}, { 0, 2, 2, 2, 2, 2, 1, 3, } },
    { { 0, 1, 1, 0, 0, 0, }, {}, { 0, 2, 2, 1, 1, 1, 1, 3, } },
    { { 1, 1, 1, 0, 1, 0, }, {}, { 0, 1, 1, 2, 2, 2, 2, 3, } },
    { { 0, 1, 1, 1, 1, 1, }, {}, { 0, 2, 2, 2, 2, 2, 2, 3, } },
    { { 1, 1, 0, 0, 1, 1, }, {}, { 0, 1, 2, 2, 2, 2, 2, 3, } },
    { { 0, 1, 0, 1, 0, 0, }, {}, { 0, 1, 1, 1, 1, 1, 1, 3, } },
  });

  auto trained_cshmm = cshmm_trainer->train(
    CSHMM::maximum_likelihood_algorithm{},
    createUntrainedDishonestCoinCasinoCSHMM(), 1);

  auto translator = tops::helper::SExprTranslator::make();
  auto serializer = trained_cshmm->serializer(translator);
  serializer->serialize();

  ASSERT_THAT(translator->sexpr(),
    Eq("(ContextSensitiveHiddenMarkovModel: "
         "(State: "
           "(DiscreteIIDModel: 0.000000 0.000000 1.000000) "
           "(DiscreteIIDModel: 0.000000 0.333333 0.333333 0.333333) "
           "(FixedDuration: fixedLength = 1)) "
         "(State: "
           "(DiscreteIIDModel: 0.528384 0.471616) "
           "(DiscreteIIDModel: 0.004329 0.562771 0.216450 0.216450) "
           "(FixedDuration: fixedLength = 1)) "
         "(State: "
           "(DiscreteIIDModel: 0.207843 0.792157) "
           "(DiscreteIIDModel: 0.003891 0.198444 0.610895 0.186770) "
           "(FixedDuration: fixedLength = 1)) "
         "(State: "
           "(DiscreteIIDModel: 0.000000 0.000000 1.000000) "
           "(DiscreteIIDModel: 0.000000 0.000000 0.000000 1.000000) "
           "(FixedDuration: fixedLength = 1)))"));
}

/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/

TEST_F(ACSHMM, CalculatesForwardAndBackwardProbabilities) {
  std::vector<Sequences> tests = {
    {{0}},
    {{1}},
    {{0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}},
  };

  for (const auto& test : tests) {
    auto[ prob_f, alphas ] = cshmm->forward(test);
    auto[ prob_b, betas ] = cshmm->backward(test);

    ASSERT_THAT(DOUBLE(prob_f), DoubleNear(DOUBLE(prob_b), 1e-7));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACSHMM, FindsTheBestPath) {
  std::vector<Sequences> tests = {
    {{0}},
    {{1}},
    {{0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}},
  };

  std::vector<CSHMM::LabelerReturn> expected = {
    { 0.0225     , {0, 1, 3}                , tests[0], {} },
    { 0.036      , {0, 2, 3}                , tests[1], {} },
    { 0.00275625 , {0, 1, 1, 1, 3}          , tests[2], {} },
    { 0.00198263 , {0, 2, 2, 2, 2, 2, 2, 3} , tests[3], {} },
  };

  for (size_t t = 0; t < tests.size(); t++) {
    auto[ estimation, label, alignment, _ ] = cshmm->viterbi(tests[t]);

    EXPECT_THAT(label, Eq(expected[t].label));
    EXPECT_THAT(alignment, Eq(expected[t].alignment));
    EXPECT_THAT(DOUBLE(estimation),
        DoubleNear(DOUBLE(expected[t].estimation), 1e-7));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACSHMM, DecodesASequenceOfObservations) {
  std::vector<Sequences> tests = {
    {{0}},
    {{1}},
    {{0, 0, 0}},
    {{1, 1, 1, 1, 1, 1}},
  };

  std::vector<CSHMM::LabelerReturn> expected = {
    { 0.714286 , {0, 1, 3}                , tests[0], {} },
    { 0.615385 , {0, 2, 3}                , tests[1], {} },
    { 0.604329 , {0, 1, 1, 1, 3}          , tests[2], {} },
    { 0.196082 , {0, 2, 2, 2, 2, 2, 2, 3} , tests[3], {} },
  };

  for (size_t t = 0; t < tests.size(); t++) {
    auto[ estimation, label, alignment, _ ]
      = cshmm->posteriorDecoding(tests[t]);

    EXPECT_THAT(label, Eq(expected[t].label));
    EXPECT_THAT(alignment, Eq(expected[t].alignment));
    EXPECT_THAT(DOUBLE(estimation),
        DoubleNear(DOUBLE(expected[t].estimation), 1e-6));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACSHMM, ShouldDrawLabeledSequenceWithDefaultSeed) {
  std::vector<size_t> tests = {
    0, 1, 2, 3, 4, 5
  };

  std::vector<CSHMM::GeneratorReturn<Sequence>> expected = {
    { { 0, 3                }, { {               } } },
    { { 0, 1, 3             }, { { 1             } } },
    { { 0, 1, 1, 3          }, { { 1, 1          } } },
    { { 0, 2, 1, 1, 3       }, { { 1, 1, 0       } } },
    { { 0, 1, 2, 2, 2, 3    }, { { 1, 1, 1, 1    } } },
    { { 0, 1, 1, 1, 2, 2, 3 }, { { 1, 0, 0, 1, 0 } } },
  };

  auto rng = RandomNumberGeneratorAdapter<std::mt19937>::make();

  for (size_t i = 0; i < tests.size(); i++) {
    auto[ label, alignment ] = cshmm->drawSequence(rng, tests[i]);

    EXPECT_THAT(label, ContainerEq(expected[i].label));
    EXPECT_THAT(alignment, ContainerEq(expected[i].alignment));
  }

  // for (auto size : { 4, 5, 6 }) {
  //   for (size_t i = 0; i < 32; i++) {
  //     auto [ label, alignment ] = cshmm->drawSequence(rng, size);
  //     std::cerr << "    { ";
  //
  //     std::cerr << "{ { ";
  //     for (auto l : alignment[0])
  //       std::cerr << l << ", ";
  //     std::cerr << "} }, ";
  //
  //     std::cerr << "{ ";
  //     for (auto l : label)
  //       std::cerr << l << ", ";
  //     std::cerr << "}";
  //
  //     std::cerr << " },";
  //     std::cerr << std::endl;
  //   }
  //   std::cerr << std::endl;
  // }
}
/*----------------------------------------------------------------------------*/

