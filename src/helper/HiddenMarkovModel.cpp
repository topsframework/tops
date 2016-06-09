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

model::HiddenMarkovModelPtr createDishonestCoinCasinoHMM() {
  std::vector<model::HiddenMarkovModel::StatePtr> states = {
    model::HiddenMarkovModel::State::make(
      0,
      model::DiscreteIIDModel::make(
        std::vector<model::Probability>{{ 0.5, 0.5 }}),
      model::DiscreteIIDModel::make(
        std::vector<model::Probability>{{ 0.7, 0.3 }})),
    model::HiddenMarkovModel::State::make(
      1,
      model::DiscreteIIDModel::make(
        std::vector<model::Probability>{{ 0.2, 0.8 }}),
      model::DiscreteIIDModel::make(
        std::vector<model::Probability>{{ 0.5, 0.5 }}))};

  return model::HiddenMarkovModel::make(
    states,
    model::DiscreteIIDModel::make(
      std::vector<model::Probability>{{ 0.9, 0.1 }}),
    2,
    2);
}

/*----------------------------------------------------------------------------*/

}  // namespace helper
}  // namespace tops
