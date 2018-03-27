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
#include "model/PhasedRunLengthDistribution.hpp"
#include "model/GeneralizedHiddenMarkovModel.hpp"

#include "model/FixedDuration.hpp"
#include "model/ExplicitDuration.hpp"

// Imports
using tops::model::FixedDuration;
using tops::model::ExplicitDuration;

// Aliases
using IID = tops::model::DiscreteIIDModel;
using PRLD = tops::model::PhasedRunLengthDistribution;
using GHMM = tops::model::GeneralizedHiddenMarkovModel;

namespace tops {
namespace app {

/*----------------------------------------------------------------------------*/
/*                                  GLOBALS                                   */
/*----------------------------------------------------------------------------*/

extern model::ProbabilisticModelPtr cds_model;
extern model::ProbabilisticModelPtr rcds_model;

extern model::DiscreteIIDModelPtr exon_single_duration;
extern model::DiscreteIIDModelPtr exon_initial_duration;
extern model::DiscreteIIDModelPtr exon_internal_duration;
extern model::DiscreteIIDModelPtr exon_terminal_duration;

/*----------------------------------------------------------------------------*/
/*                               FORWARD STRAND                               */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_ES() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["stop"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_single_duration, 4, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["ES"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_EI0() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don1"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_initial_duration, 11, 0, 0, 3));

  return GHMM::MatchState::make(
      state_indices["EI0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_EI1() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don2"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_initial_duration, 11, 0, 1, 3));

  return GHMM::MatchState::make(
      state_indices["EI1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_EI2() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don0"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_initial_duration, 11, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["EI2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_E00() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don1"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 0, 3));

  return GHMM::MatchState::make(
      state_indices["E00"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_E01() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don2"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 1, 3));

  return GHMM::MatchState::make(
      state_indices["E01"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_E02() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don2"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["E02"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_E10() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don1"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["E10"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_E11() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don2"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 0, 3));

  return GHMM::MatchState::make(
      state_indices["E11"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_E12() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don0"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 1, 3));

  return GHMM::MatchState::make(
      state_indices["E12"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_E20() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don1"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 1, 3));

  return GHMM::MatchState::make(
      state_indices["E20"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_E21() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don2"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["E21"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_E22() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["don0"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 0, 3));

  return GHMM::MatchState::make(
      state_indices["E22"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_ET0() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["stop"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_terminal_duration, 5, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["ET0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_ET1() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["stop"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_terminal_duration, 5, 1, 2, 3));

  return GHMM::MatchState::make(
      state_indices["ET1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_ET2() {
  auto& emission_model = cds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["stop"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_terminal_duration, 5, 2, 2, 3));

  return GHMM::MatchState::make(
      state_indices["ET2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/
/*                               REVERSE STRAND                               */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rES() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["rstart"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_single_duration, 4, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["rES"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rEI0() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["rstart"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_initial_duration, 11, 0, 0, 3));

  return GHMM::MatchState::make(
      state_indices["rEI0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rEI1() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["rstart"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_initial_duration, 11, 0, 1, 3));

  return GHMM::MatchState::make(
      state_indices["rEI1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rEI2() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["rstart"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_initial_duration, 11, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["rEI2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rE00() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc0"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 0, 3));

  return GHMM::MatchState::make(
      state_indices["rE00"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rE01() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc1"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["rE01"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rE02() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc2"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 1, 3));

  return GHMM::MatchState::make(
      state_indices["rE02"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rE10() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc0"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 1, 3));

  return GHMM::MatchState::make(
      state_indices["rE10"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rE11() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc1"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 0, 3));

  return GHMM::MatchState::make(
      state_indices["rE11"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rE12() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc2"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["rE12"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rE20() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc0"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["rE20"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rE21() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc1"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 1, 3));

  return GHMM::MatchState::make(
      state_indices["rE21"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rE22() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc2"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_internal_duration, 12, 0, 0, 3));

  return GHMM::MatchState::make(
      state_indices["rE22"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rET0() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc0"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_terminal_duration, 5, 0, 2, 3));

  return GHMM::MatchState::make(
      state_indices["rET0"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rET1() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc1"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_terminal_duration, 5, 1, 2, 3));

  return GHMM::MatchState::make(
      state_indices["rET1"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_rET2() {
  auto& emission_model = rcds_model;

  auto transition_model
    = IID::make(index_probabilities({ state_indices["racc2"] }, { 1 }));

  auto duration_model
    = ExplicitDuration::make(
        PRLD::makeFromDiscreteIIDModel(exon_terminal_duration, 5, 2, 2, 3));

  return GHMM::MatchState::make(
      state_indices["rET2"], emission_model, transition_model, duration_model);
}

/*----------------------------------------------------------------------------*/

}  // namespace app
}  // namespace tops
