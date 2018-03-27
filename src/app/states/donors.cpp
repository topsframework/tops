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

#include "model/FixedDuration.hpp"

// Imports
using tops::model::FixedDuration;

// Aliases
using IID = tops::model::DiscreteIIDModel;
using GHMM = tops::model::GeneralizedHiddenMarkovModel;

namespace tops {
namespace app {

/*----------------------------------------------------------------------------*/
/*                                  GLOBALS                                   */
/*----------------------------------------------------------------------------*/

extern model::ProbabilisticModelPtr donor_model;
extern model::ProbabilisticModelPtr rdonor_model;

/*----------------------------------------------------------------------------*/
/*                               FORWARD STRAND                               */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_don0() {
  std::vector<model::Probability> transition_probabilities {
    0.144160536105428,
    0.855839463894572,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["If0"],
    state_indices["Is0"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = donor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(13);

  return GHMM::MatchState::make(
      state_indices["don0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_don1() {
  std::vector<model::Probability> transition_probabilities {
    0.144160536105428,
    0.855839463894572,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["If1"],
    state_indices["Is1"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = donor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(13);

  return GHMM::MatchState::make(
      state_indices["don1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_don2() {
  std::vector<model::Probability> transition_probabilities {
    0.144160536105428,
    0.855839463894572,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["If2"],
    state_indices["Is2"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = donor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(13);

  return GHMM::MatchState::make(
      state_indices["don2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/
/*                               REVERSE STRAND                               */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rdon0() {
  std::vector<model::Probability> transition_probabilities {
    0.3,
    0.3,
    0.3,
    0.1,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["rE20"],
    state_indices["rE21"],
    state_indices["rE22"],
    state_indices["rEI2"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = rdonor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(13);

  return GHMM::MatchState::make(
      state_indices["rdon0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rdon1() {
  std::vector<model::Probability> transition_probabilities {
    0.3,
    0.3,
    0.3,
    0.1,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["rE00"],
    state_indices["rE01"],
    state_indices["rE02"],
    state_indices["rEI0"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = rdonor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(13);

  return GHMM::MatchState::make(
      state_indices["rdon1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rdon2() {
  std::vector<model::Probability> transition_probabilities {
    0.3,
    0.3,
    0.3,
    0.1,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["rE10"],
    state_indices["rE11"],
    state_indices["rE12"],
    state_indices["rEI1"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = rdonor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(13);

  return GHMM::MatchState::make(
      state_indices["rdon2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

}  // namespace app
}  // namespace tops
