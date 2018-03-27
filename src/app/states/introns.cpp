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
#include "model/ExplicitDuration.hpp"

// Imports
using tops::model::FixedDuration;
using tops::model::ExplicitDuration;

// Aliases
using IID = tops::model::DiscreteIIDModel;
using GHMM = tops::model::GeneralizedHiddenMarkovModel;

namespace tops {
namespace app {

/*----------------------------------------------------------------------------*/
/*                                  GLOBALS                                   */
/*----------------------------------------------------------------------------*/

extern model::ProbabilisticModelPtr non_coding_model;
extern model::DiscreteIIDModelPtr short_duration;

/*----------------------------------------------------------------------------*/
/*                               FORWARD STRAND                               */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_I0() {
  std::vector<model::Probability> transition_probabilities {
    0.998908491611056,
    0.00109150838894445,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["I0"],
    state_indices["acc0"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = non_coding_model;
  auto transition_model
    = IID::make(transition_indexed_probabilities);

  return GHMM::MatchState::make(
      state_indices["I0"], emission_model, transition_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_I1() {
  std::vector<model::Probability> transition_probabilities {
    0.998908491611056,
    0.00109150838894445,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["I1"],
    state_indices["acc1"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = non_coding_model;
  auto transition_model
    = IID::make(transition_indexed_probabilities);

  return GHMM::MatchState::make(
      state_indices["I1"], emission_model, transition_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_I2() {
  std::vector<model::Probability> transition_probabilities {
    0.998908491611056,
    0.00109150838894445,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["I2"],
    state_indices["acc2"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = non_coding_model;
  auto transition_model
    = IID::make(transition_indexed_probabilities);

  return GHMM::MatchState::make(
      state_indices["I2"], emission_model, transition_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_Is0() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["acc0"] }, { 1 }));

  auto duration_model = ExplicitDuration::make(short_duration);

  return GHMM::MatchState::make(
      state_indices["Is0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_Is1() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["acc1"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(short_duration);

  return GHMM::MatchState::make(
      state_indices["Is1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_Is2() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["acc2"] }, { 1 }));

  auto duration_model = ExplicitDuration::make(short_duration);

  return GHMM::MatchState::make(
      state_indices["Is2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_If0() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["I0"] }, { 1 }));
  auto duration_model
    = FixedDuration::make(584);

  return GHMM::MatchState::make(
      state_indices["If0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_If1() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["I1"] }, { 1 }));

  auto duration_model
    = FixedDuration::make(584);

  return GHMM::MatchState::make(
      state_indices["If1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_If2() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["I2"] }, { 1 }));

  auto duration_model
    = FixedDuration::make(584);

  return GHMM::MatchState::make(
      state_indices["If2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/
/*                               REVERSE STRAND                               */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rI0() {
  std::vector<model::Probability> transition_probabilities {
    0.998908491611056,
    0.00109150838894445,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["rI0"],
    state_indices["rdon0"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = non_coding_model;
  auto transition_model
    = IID::make(transition_indexed_probabilities);

  return GHMM::MatchState::make(
      state_indices["rI0"], emission_model, transition_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rI1() {
  std::vector<model::Probability> transition_probabilities {
    0.998908491611056,
    0.00109150838894445,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["rI1"],
    state_indices["rdon1"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = non_coding_model;
  auto transition_model
    = IID::make(transition_indexed_probabilities);

  return GHMM::MatchState::make(
      state_indices["rI1"], emission_model, transition_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rI2() {
  std::vector<model::Probability> transition_probabilities {
    0.998908491611056,
    0.00109150838894445,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["rI2"],
    state_indices["rdon2"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = non_coding_model;
  auto transition_model
    = IID::make(transition_indexed_probabilities);

  return GHMM::MatchState::make(
      state_indices["rI2"], emission_model, transition_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rIs0() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["rdon0"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(short_duration);

  return GHMM::MatchState::make(
      state_indices["rIs0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rIs1() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["rdon1"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(short_duration);

  return GHMM::MatchState::make(
      state_indices["rIs1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rIs2() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["rdon2"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(short_duration);

  return GHMM::MatchState::make(
      state_indices["rIs2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rIf0() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["rI0"] }, { 1 }));

  auto duration_model
    = FixedDuration::make(584);

  return GHMM::MatchState::make(
      state_indices["rIf0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rIf1() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["rI1"] }, { 1 }));

  auto duration_model
    = FixedDuration::make(584);

  return GHMM::MatchState::make(
      state_indices["rIf1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rIf2() {
  auto& emission_model = non_coding_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["rI2"] }, { 1 }));

  auto duration_model
    = FixedDuration::make(584);

  return GHMM::MatchState::make(
      state_indices["rIf2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

}  // namespace app
}  // namespace tops
