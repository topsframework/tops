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
#include "app/states.hpp"

// Internal headers
#include "app/training.hpp"

#include "model/Probability.hpp"

#include "model/ProbabilisticModel.hpp"
#include "model/DiscreteIIDModel.hpp"
#include "model/GeneralizedHiddenMarkovModel.hpp"

#include "model/SignalDuration.hpp"

// Imports
using tops::model::SignalDuration;

// Aliases
using IID = tops::model::DiscreteIIDModel;
using GHMM = tops::model::GeneralizedHiddenMarkovModel;

namespace tops {
namespace app {

/*----------------------------------------------------------------------------*/
/*                                  GLOBALS                                   */
/*----------------------------------------------------------------------------*/

extern model::ProbabilisticModelPtr stop_model;
extern model::ProbabilisticModelPtr rstop_model;

/*----------------------------------------------------------------------------*/
/*                               FORWARD STRAND                               */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_stop() {
  auto& emission_model = stop_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["N"] }, { 1 }));

  auto duration_model = SignalDuration::make(3);

  return GHMM::State::make(
      state_indices["rstop"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/
/*                               REVERSE STRAND                               */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rstop() {
  std::vector<model::Probability> transition_probabilities {
    0.3,
    0.3,
    0.3,
    0.1,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["rET0"],
    state_indices["rET1"],
    state_indices["rET2"],
    state_indices["rES"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = stop_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = SignalDuration::make(3);

  return GHMM::State::make(
      state_indices["rstop"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

}  // namespace app
}  // namespace tops
