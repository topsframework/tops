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
#include "myop/states.hpp"

// Standard headers
#include <vector>

// Internal headers
#include "myop/training.hpp"

#include "model/Probability.hpp"

#include "model/ProbabilisticModel.hpp"
#include "model/DiscreteIIDModel.hpp"
#include "model/GeneralizedHiddenMarkovModel.hpp"

#include "model/FixedDuration.hpp"

// Imports
using tops::model::FixedDuration;

// Aliases
using IID = tops::model::DiscreteIIDModel;
using GHMM = tops::model::GeneralizedHiddenMarkovModel;

namespace tops {
namespace myop {

/*----------------------------------------------------------------------------*/
/*                                  GLOBALS                                   */
/*----------------------------------------------------------------------------*/

extern model::ProbabilisticModelPtr non_coding_model;

/*----------------------------------------------------------------------------*/
/*                                BOTH STRANDS                                */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_B() {
  auto emission_model
    = IID::make(model::Probabilities(nucleotides->size(), 1.0));

  auto transition_model
    = IID::make(index_probabilities({ state_indices["N"] }, { 1.0 }));

  auto duration_model
    = FixedDuration::make(0);

  return GHMM::SilentState::make(
      state_indices["B"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_N() {
  std::vector<model::Probability> transition_probabilities {
    0.999882352941176,
    9.0735294117647e-06,
    9.0735294117647e-06,
    9.95e-05,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["N"],
    state_indices["start"],
    state_indices["rstop"],
    state_indices["E"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model
    = non_coding_model;
  auto transition_model
    = IID::make(transition_indexed_probabilities);

  return GHMM::MatchState::make(
      state_indices["N"], emission_model, transition_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_E() {
  auto emission_model
    = IID::make(model::Probabilities(nucleotides->size(), 1.0));

  auto transition_model
    = IID::make(index_probabilities({ state_indices["E"] }, { 1 }));

  auto duration_model
    = FixedDuration::make(0);

  return GHMM::SilentState::make(
      state_indices["E"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

}  // namespace myop
}  // namespace tops
