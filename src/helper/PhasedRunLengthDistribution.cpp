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

// ToPS headers
#include "helper/Sequence.hpp"

// Interface header
#include "helper/PhasedRunLengthDistribution.hpp"

namespace tops {
namespace helper {

/*----------------------------------------------------------------------------*/
/*                                 FUNCTIONS                                  */
/*----------------------------------------------------------------------------*/

tops::model::PhasedRunLengthDistributionPtr
createSamplePhasedRunLengthDistribution() {
  auto iid_trainer = tops::model::DiscreteIIDModel::standardTrainer(
    tops::model::DiscreteIIDModel::smoothed_histogram_burge_algorithm{},
    1.0, 15000);
  iid_trainer->add_training_sequence(sequenceOfLengths());

  return tops::model::PhasedRunLengthDistribution::makeFromDiscreteIIDModel(
    iid_trainer->train(), 12, 0, 1, 3);
}

/*----------------------------------------------------------------------------*/

}  // namespace helper
}  // namespace tops
