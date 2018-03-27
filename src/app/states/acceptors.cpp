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

// Standard headers
#include <vector>

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

extern model::ProbabilisticModelPtr acceptor_model;
extern model::ProbabilisticModelPtr racceptor_model;

/*----------------------------------------------------------------------------*/
/*                               FORWARD STRAND                               */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_acc0() {
  std::vector<model::Probability> transition_probabilities {
    0.3,
    0.3,
    0.3,
    0.1,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["E00"],
    state_indices["E01"],
    state_indices["E02"],
    state_indices["ET0"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = acceptor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(42);

  return GHMM::MatchState::make(
      state_indices["acc0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_acc1() {
  std::vector<model::Probability> transition_probabilities {
    0.3,
    0.3,
    0.3,
    0.1,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["E10"],
    state_indices["E11"],
    state_indices["E12"],
    state_indices["ET1"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = acceptor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(42);

  return GHMM::MatchState::make(
      state_indices["acc1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_acc2() {
  std::vector<model::Probability> transition_probabilities {
    0.3,
    0.3,
    0.3,
    0.1,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["E20"],
    state_indices["E21"],
    state_indices["E22"],
    state_indices["ET2"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = acceptor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(42);

  return GHMM::MatchState::make(
      state_indices["acc2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/
/*                               REVERSE STRAND                               */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_racc0() {
  std::vector<model::Probability> transition_probabilities {
    0.144160536105428,
    0.855839463894572,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["rIf0"],
    state_indices["rIs0"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = racceptor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(42);

  return GHMM::MatchState::make(
      state_indices["racc0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_racc1() {
  std::vector<model::Probability> transition_probabilities {
    0.144160536105428,
    0.855839463894572,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["rIf1"],
    state_indices["rIs1"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = racceptor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(42);

  return GHMM::MatchState::make(
      state_indices["racc1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_racc2() {
  std::vector<model::Probability> transition_probabilities {
    0.144160536105428,
    0.855839463894572,
  };

  std::vector<GHMM::State::Id> transition_indices = {
    state_indices["rIf2"],
    state_indices["rIs2"],
  };

  auto transition_indexed_probabilities
    = index_probabilities(transition_indices, transition_probabilities);

  auto& emission_model = racceptor_model;
  auto transition_model = IID::make(transition_indexed_probabilities);
  auto duration_model = FixedDuration::make(42);

  return GHMM::MatchState::make(
      state_indices["racc2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

}  // namespace app
}  // namespace tops
