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

#ifndef TOPS_APP_STATES_
#define TOPS_APP_STATES_

// Standard headers
#include <string>
#include <vector>
#include <unordered_map>

// Internal headers
#include "app/Alphabet.hpp"

#include "model/Symbol.hpp"
#include "model/Probability.hpp"
#include "model/GeneralizedHiddenMarkovModel.hpp"

namespace tops {
namespace app {

namespace {

// Imports
using model::Symbol;
using model::Probability;

// Aliases
using IID = model::DiscreteIIDModel;
using GHMM = model::GeneralizedHiddenMarkovModel;

}  // namespace

/*----------------------------------------------------------------------------*/
/*                                  GLOBALS                                   */
/*----------------------------------------------------------------------------*/

extern std::shared_ptr<Alphabet> nucleotides;
extern std::vector<std::string> state_names;
extern std::unordered_map<std::string, Symbol> state_indices;

void initialize_globals(const std::string& dataset);

/*----------------------------------------------------------------------------*/
/*                                AUXILIARIES                                 */
/*----------------------------------------------------------------------------*/

std::vector<Probability> index_probabilities(
    const std::vector<GHMM::State::Id>& indices,
    const std::vector<Probability>& probabilities);

void print_probabilities(const std::vector<Probability>& probabilities);

/*----------------------------------------------------------------------------*/
/*                                BOTH STRANDS                                */
/*----------------------------------------------------------------------------*/

GHMM::StatePtr make_state_N();
GHMM::StatePtr make_state_F();

/*----------------------------------------------------------------------------*/
/*                               FORWARD STRAND                               */
/*----------------------------------------------------------------------------*/

// Start
GHMM::StatePtr make_state_start();

// Exons
GHMM::StatePtr make_state_ES();

GHMM::StatePtr make_state_EI0();
GHMM::StatePtr make_state_EI1();
GHMM::StatePtr make_state_EI2();

GHMM::StatePtr make_state_E00();
GHMM::StatePtr make_state_E01();
GHMM::StatePtr make_state_E02();

GHMM::StatePtr make_state_E10();
GHMM::StatePtr make_state_E11();
GHMM::StatePtr make_state_E12();

GHMM::StatePtr make_state_E20();
GHMM::StatePtr make_state_E21();
GHMM::StatePtr make_state_E22();

GHMM::StatePtr make_state_ET0();
GHMM::StatePtr make_state_ET1();
GHMM::StatePtr make_state_ET2();

// Donors
GHMM::StatePtr make_state_don0();
GHMM::StatePtr make_state_don1();
GHMM::StatePtr make_state_don2();

// Introns
GHMM::StatePtr make_state_I0();
GHMM::StatePtr make_state_I1();
GHMM::StatePtr make_state_I2();

GHMM::StatePtr make_state_Is0();
GHMM::StatePtr make_state_Is1();
GHMM::StatePtr make_state_Is2();

GHMM::StatePtr make_state_If0();
GHMM::StatePtr make_state_If1();
GHMM::StatePtr make_state_If2();

// Acceptors
GHMM::StatePtr make_state_acc0();
GHMM::StatePtr make_state_acc1();
GHMM::StatePtr make_state_acc2();

// Stop
GHMM::StatePtr make_state_stop();

/*----------------------------------------------------------------------------*/
/*                               REVERSE STRAND                               */
/*----------------------------------------------------------------------------*/

// Stop
GHMM::StatePtr make_state_rstop();

// Exons
GHMM::StatePtr make_state_rES();

GHMM::StatePtr make_state_rEI0();
GHMM::StatePtr make_state_rEI1();
GHMM::StatePtr make_state_rEI2();

GHMM::StatePtr make_state_rE00();
GHMM::StatePtr make_state_rE01();
GHMM::StatePtr make_state_rE02();

GHMM::StatePtr make_state_rE10();
GHMM::StatePtr make_state_rE11();
GHMM::StatePtr make_state_rE12();

GHMM::StatePtr make_state_rE20();
GHMM::StatePtr make_state_rE21();
GHMM::StatePtr make_state_rE22();

GHMM::StatePtr make_state_rET0();
GHMM::StatePtr make_state_rET1();
GHMM::StatePtr make_state_rET2();

// Acceptors
GHMM::StatePtr make_state_racc0();
GHMM::StatePtr make_state_racc1();
GHMM::StatePtr make_state_racc2();

// Introns
GHMM::StatePtr make_state_rI0();
GHMM::StatePtr make_state_rI1();
GHMM::StatePtr make_state_rI2();

GHMM::StatePtr make_state_rIs0();
GHMM::StatePtr make_state_rIs1();
GHMM::StatePtr make_state_rIs2();

GHMM::StatePtr make_state_rIf0();
GHMM::StatePtr make_state_rIf1();
GHMM::StatePtr make_state_rIf2();

// Donors
GHMM::StatePtr make_state_rdon0();
GHMM::StatePtr make_state_rdon1();
GHMM::StatePtr make_state_rdon2();

// Start
GHMM::StatePtr make_state_rstart();

}  // namespace app
}  // namespace tops

#endif  // TOPS_APP_STATES_
