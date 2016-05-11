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

// Interface header
#include "helper/HiddenMarkovModel.hpp"

// Standard headers
#include <cmath>
#include <vector>

// Internal headers
#include "helper/DiscreteIIDModel.hpp"

#include "model/Probability.hpp"

namespace tops {
namespace helper {

/*----------------------------------------------------------------------------*/
/*                                 FUNCTIONS                                  */
/*----------------------------------------------------------------------------*/

tops::model::HiddenMarkovModelPtr createDishonestCoinCasinoHMM() {
  std::vector<tops::model::HiddenMarkovModel::StatePtr> states = {
    tops::model::HiddenMarkovModel::State::make(
      0,
      tops::model::DiscreteIIDModel::make(
        std::vector<tops::model::Probability>{ log(0.5), log(0.5) }),
      tops::model::DiscreteIIDModel::make(
        std::vector<tops::model::Probability>{ log(0.7), log(0.3) })),
    tops::model::HiddenMarkovModel::State::make(
      1,
      tops::model::DiscreteIIDModel::make(
        std::vector<tops::model::Probability>{ log(0.2), log(0.8) }),
      tops::model::DiscreteIIDModel::make(
        std::vector<tops::model::Probability>{ log(0.5), log(0.5) }))};

  return tops::model::HiddenMarkovModel::make(
    states,
    tops::model::DiscreteIIDModel::make(
      std::vector<tops::model::Probability>{ log(0.9), log(0.1) }),
    2,
    2);
}

/*----------------------------------------------------------------------------*/

}  // namespace helper
}  // namespace tops
