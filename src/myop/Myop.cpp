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
#include "myop/Myop.hpp"
#include <iostream>

// Internal headers
#include "myop/states.hpp"

#include "model/Sequence.hpp"
#include "model/Labeling.hpp"
#include "model/Estimation.hpp"

#include "model/Labeler.hpp"

#include "model/DiscreteIIDModel.hpp"
#include "model/GeneralizedHiddenMarkovModel.hpp"

namespace tops {
namespace myop {

/*----------------------------------------------------------------------------*/
/*                              LOCAL FUNCTIONS                               */
/*----------------------------------------------------------------------------*/

void connect_states(GHMM::StatePtr successor, GHMM::StatePtr predecessor) {
    predecessor->addSuccessor(successor->id());
    successor->addPredecessor(predecessor->id());
}

/*----------------------------------------------------------------------------*/
/*                              STATIC VARIABLES                              */
/*----------------------------------------------------------------------------*/

const std::size_t Myop::alphabetSize = 4;

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

Myop::Myop(std::string dataset) {
  tops::myop::initialize_globals(dataset);

  std::cerr << "##############################################" << std::endl;
  std::cerr << "##                  STATES                  ##" << std::endl;
  std::cerr << "##############################################" << std::endl;
  std::cerr << std::endl;

  std::cerr << "Making states ..." << std::flush;

  /*--------------------------------------------------------------------------*/
  /*                               BOTH STRANDS                               */
  /*--------------------------------------------------------------------------*/

  auto B = tops::myop::make_state_B();
  auto N = tops::myop::make_state_N();
  auto E = tops::myop::make_state_E();

  /*--------------------------------------------------------------------------*/
  /*                              FORWARD STRAND                              */
  /*--------------------------------------------------------------------------*/

  // Start
  auto start = tops::myop::make_state_start();

  // Exons
  auto ES = tops::myop::make_state_ES();

  auto EI0 = tops::myop::make_state_EI0();
  auto EI1 = tops::myop::make_state_EI1();
  auto EI2 = tops::myop::make_state_EI2();

  auto E00 = tops::myop::make_state_E00();
  auto E01 = tops::myop::make_state_E01();
  auto E02 = tops::myop::make_state_E02();

  auto E10 = tops::myop::make_state_E10();
  auto E11 = tops::myop::make_state_E11();
  auto E12 = tops::myop::make_state_E12();

  auto E20 = tops::myop::make_state_E20();
  auto E21 = tops::myop::make_state_E21();
  auto E22 = tops::myop::make_state_E22();

  auto ET0 = tops::myop::make_state_ET0();
  auto ET1 = tops::myop::make_state_ET1();
  auto ET2 = tops::myop::make_state_ET2();

  // Donors
  auto don0 = tops::myop::make_state_don0();
  auto don1 = tops::myop::make_state_don1();
  auto don2 = tops::myop::make_state_don2();

  // Introns
  auto I0 = tops::myop::make_state_I0();
  auto I1 = tops::myop::make_state_I1();
  auto I2 = tops::myop::make_state_I2();

  auto Is0 = tops::myop::make_state_Is0();
  auto Is1 = tops::myop::make_state_Is1();
  auto Is2 = tops::myop::make_state_Is2();

  auto If0 = tops::myop::make_state_If0();
  auto If1 = tops::myop::make_state_If1();
  auto If2 = tops::myop::make_state_If2();

  // Acceptors
  auto acc0 = tops::myop::make_state_acc0();
  auto acc1 = tops::myop::make_state_acc1();
  auto acc2 = tops::myop::make_state_acc2();

  // Stop
  auto stop = tops::myop::make_state_stop();

  /*--------------------------------------------------------------------------*/
  /*                              REVERSE STRAND                              */
  /*--------------------------------------------------------------------------*/

  // Stop
  auto rstop = tops::myop::make_state_rstop();

  // Exons
  auto rES = tops::myop::make_state_rES();

  auto rEI0 = tops::myop::make_state_rEI0();
  auto rEI1 = tops::myop::make_state_rEI0();
  auto rEI2 = tops::myop::make_state_rEI0();

  auto rE00 = tops::myop::make_state_rE00();
  auto rE01 = tops::myop::make_state_rE01();
  auto rE02 = tops::myop::make_state_rE02();

  auto rE10 = tops::myop::make_state_rE10();
  auto rE11 = tops::myop::make_state_rE11();
  auto rE12 = tops::myop::make_state_rE12();

  auto rE20 = tops::myop::make_state_rE20();
  auto rE21 = tops::myop::make_state_rE21();
  auto rE22 = tops::myop::make_state_rE22();

  auto rET0 = tops::myop::make_state_rET0();
  auto rET1 = tops::myop::make_state_rET1();
  auto rET2 = tops::myop::make_state_rET2();

  // Acceptors
  auto racc0 = tops::myop::make_state_racc0();
  auto racc1 = tops::myop::make_state_racc1();
  auto racc2 = tops::myop::make_state_racc2();

  // Introns
  auto rI0 = tops::myop::make_state_rI0();
  auto rI1 = tops::myop::make_state_rI1();
  auto rI2 = tops::myop::make_state_rI2();

  auto rIs0 = tops::myop::make_state_rIs0();
  auto rIs1 = tops::myop::make_state_rIs1();
  auto rIs2 = tops::myop::make_state_rIs2();

  auto rIf0 = tops::myop::make_state_rIf0();
  auto rIf1 = tops::myop::make_state_rIf1();
  auto rIf2 = tops::myop::make_state_rIf2();

  // Donors
  auto rdon0 = tops::myop::make_state_rdon0();
  auto rdon1 = tops::myop::make_state_rdon1();
  auto rdon2 = tops::myop::make_state_rdon2();

  // Start
  auto rstart = tops::myop::make_state_rstart();
  std::cerr << " OK" << std::endl;

  std::cerr << "Making GHMM ..." << std::flush;
  /*--------------------------------------------------------------------------*/
  /*                                   GHMM                                   */
  /*--------------------------------------------------------------------------*/

  std::vector<GHMM::StatePtr> states {
    B,

    start,
    ES,
    EI0, EI1, EI2,
    E00, E01, E02,
    E10, E11, E12,
    E20, E21, E22,
    ET0, ET1, ET2,
    don0, don1, don2,
    I0, I1, I2,
    Is0, If0, Is1,
    If1,  Is2, If2,
    acc0, acc1, acc2,
    stop,

    N,

    rstop,
    rES,
    rEI0, rEI1, rEI2,
    rE00, rE01, rE02,
    rE10, rE11, rE12,
    rE20, rE21, rE22,
    rET0, rET1, rET2,
    racc0, racc1, racc2,
    rI0, rI1, rI2,
    rIs0, rIf0, rIs1,
    rIf1, rIs2, rIf2,
    rdon0, rdon1, rdon2,
    rstart,

    E
  };

  connect_states(don1, E00);
  connect_states(don2, E01);
  connect_states(don0, E02);
  connect_states(don1, E10);
  connect_states(don2, E11);
  connect_states(don0, E12);
  connect_states(don1, E20);
  connect_states(don2, E21);
  connect_states(don0, E22);
  connect_states(don1, EI0);
  connect_states(don2, EI1);
  connect_states(don0, EI2);
  connect_states(stop, ES);
  connect_states(stop, ET0);
  connect_states(stop, ET1);
  connect_states(stop, ET2);
  connect_states(E, E);
  connect_states(I0, I0);
  connect_states(acc0, I0);
  connect_states(I1, I1);
  connect_states(acc1, I1);
  connect_states(I2, I2);
  connect_states(acc2, I2);
  connect_states(I0, If0);
  connect_states(I1, If1);
  connect_states(I2, If2);
  connect_states(acc0, Is0);
  connect_states(acc1, Is1);
  connect_states(acc2, Is2);
  connect_states(N, B);
  connect_states(N, N);
  connect_states(E, N);
  connect_states(rstop, N);
  connect_states(start, N);
  connect_states(E00, acc0);
  connect_states(E01, acc0);
  connect_states(E02, acc0);
  connect_states(ET0, acc0);
  connect_states(E10, acc1);
  connect_states(E11, acc1);
  connect_states(E12, acc1);
  connect_states(ET1, acc1);
  connect_states(E20, acc2);
  connect_states(E21, acc2);
  connect_states(E22, acc2);
  connect_states(ET2, acc2);
  connect_states(If0, don0);
  connect_states(Is0, don0);
  connect_states(If1, don1);
  connect_states(Is1, don1);
  connect_states(If2, don2);
  connect_states(Is2, don2);
  connect_states(racc0, rE00);
  connect_states(racc1, rE01);
  connect_states(racc2, rE02);
  connect_states(racc0, rE10);
  connect_states(racc1, rE11);
  connect_states(racc2, rE12);
  connect_states(racc0, rE20);
  connect_states(racc1, rE21);
  connect_states(racc2, rE22);
  connect_states(rstart, rEI0);
  connect_states(rstart, rEI1);
  connect_states(rstart, rEI2);
  connect_states(rstart, rES);
  connect_states(racc0, rET0);
  connect_states(racc1, rET1);
  connect_states(racc2, rET2);
  connect_states(rI0, rI0);
  connect_states(rdon0, rI0);
  connect_states(rI1, rI1);
  connect_states(rdon1, rI1);
  connect_states(rI2, rI2);
  connect_states(rdon2, rI2);
  connect_states(rI0, rIf0);
  connect_states(rI1, rIf1);
  connect_states(rI2, rIf2);
  connect_states(rdon0, rIs0);
  connect_states(rdon1, rIs1);
  connect_states(rdon2, rIs2);
  connect_states(rIf0, racc0);
  connect_states(rIs0, racc0);
  connect_states(rIf1, racc1);
  connect_states(rIs1, racc1);
  connect_states(rIf2, racc2);
  connect_states(rIs2, racc2);
  connect_states(rE20, rdon0);
  connect_states(rE21, rdon0);
  connect_states(rE22, rdon0);
  connect_states(rEI2, rdon0);
  connect_states(rE00, rdon1);
  connect_states(rE01, rdon1);
  connect_states(rE02, rdon1);
  connect_states(rEI0, rdon1);
  connect_states(rE10, rdon2);
  connect_states(rE11, rdon2);
  connect_states(rE12, rdon2);
  connect_states(rEI1, rdon2);
  connect_states(N, rstart);
  connect_states(rES, rstop);
  connect_states(rET0, rstop);
  connect_states(rET1, rstop);
  connect_states(rET2, rstop);
  connect_states(EI0, start);
  connect_states(EI1, start);
  connect_states(EI2, start);
  connect_states(ES, start);
  connect_states(N, stop);

  _ghmm = GHMM::make(states, states.size(), Myop::alphabetSize, 3, 15'000);

  std::cerr << " OK" << std::endl;
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

model::Sequence Myop::predict(const model::Sequence& observation, const model::Probabilities extrinsic_probabilities) {
  // auto labeler = _ghmm->labeler(observation, true);
  // auto estimation = labeler->labeling(model::Labeler::method::bestPath);
  auto [ estimation, label, alignment, _ ] = _ghmm->viterbi({ observation }, extrinsic_probabilities);

  return label;
}

/*----------------------------------------------------------------------------*/

}  // namespace myop
}  // namespace tops
