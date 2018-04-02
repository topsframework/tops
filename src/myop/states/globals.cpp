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
#include <iostream>
#include <memory>

// Internal headers
#include "myop/training.hpp"

#include "model/ProbabilisticModel.hpp"
#include "model/DiscreteIIDModel.hpp"

namespace tops {
namespace myop {

/*----------------------------------------------------------------------------*/
/*                                  GLOBALS                                   */
/*----------------------------------------------------------------------------*/

// Globals
std::shared_ptr<Alphabet> nucleotides;
std::vector<std::string> state_names;
std::unordered_map<std::string, Symbol> state_indices;

// Both
model::ProbabilisticModelPtr non_coding_model;

// Start
model::ProbabilisticModelPtr start_model;
model::ProbabilisticModelPtr rstart_model;

// Exons
model::ProbabilisticModelPtr cds_model;
model::ProbabilisticModelPtr rcds_model;

model::DiscreteIIDModelPtr exon_single_duration;
model::DiscreteIIDModelPtr exon_initial_duration;
model::DiscreteIIDModelPtr exon_internal_duration;
model::DiscreteIIDModelPtr exon_terminal_duration;

// Donors
model::ProbabilisticModelPtr donor_model;
model::ProbabilisticModelPtr rdonor_model;

// Intron
model::ProbabilisticModelPtr short_duration;

// Acceptors
model::ProbabilisticModelPtr acceptor_model;
model::ProbabilisticModelPtr racceptor_model;

// Stop
model::ProbabilisticModelPtr stop_model;
model::ProbabilisticModelPtr rstop_model;

/*----------------------------------------------------------------------------*/
/*                                 FUNCTIONS                                  */
/*----------------------------------------------------------------------------*/

void initialize_globals(const std::string& dataset) {
  tops::myop::DATASET = dataset;

  nucleotides = std::make_shared<Alphabet>();
  nucleotides->addSymbol("A");
  nucleotides->addSymbol("C");
  nucleotides->addSymbol("G");
  nucleotides->addSymbol("T");

  state_names = std::vector<std::string>{
    "B",

    "start",
    "ES",
    "EI0", "EI1", "EI2",
    "E00", "E01", "E02",
    "E10", "E11", "E12",
    "E20", "E21", "E22",
    "ET0", "ET1", "ET2",
    "don0", "don1", "don2",
    "I0", "I1", "I2",
    "Is0", "If0", "Is1",
    "If1",  "Is2", "If2",
    "acc0", "acc1", "acc2",
    "stop",

    "N",

    "rstop",
    "rES",
    "rEI0", "rEI1", "rEI2",
    "rE00", "rE01", "rE02",
    "rE10", "rE11", "rE12",
    "rE20", "rE21", "rE22",
    "rET0", "rET1", "rET2",
    "racc0", "racc1", "racc2",
    "rI0", "rI1", "rI2",
    "rIs0", "rIf0", "rIs1",
    "rIf1", "rIs2", "rIf2",
    "rdon0", "rdon1", "rdon2",
    "rstart",

    "E"
  };

  for (std::size_t index = 0; index < state_names.size(); index++)
    state_indices.emplace(state_names[index], index);

  std::cerr << "##############################################" << std::endl;
  std::cerr << "##                 TRAINING                 ##" << std::endl;
  std::cerr << "##############################################" << std::endl;
  std::cerr << std::endl;

  // Both
  std::cerr << "Training NonCoding models..." << std::flush;
  non_coding_model = trainNonCodingModel(nucleotides);
  std::cerr << " OK" << std::endl;

  // Start
  std::cerr << "Training Start models..." << std::flush;
  start_model = trainStartModel(nucleotides);
  rstart_model = trainRStartModel(nucleotides);
  std::cerr << " OK" << std::endl;

  // Exons
  std::cerr << "Training CDS models..." << std::flush;
  cds_model = trainCDSModel(nucleotides);
  rcds_model = trainRCDSModel(nucleotides);
  std::cerr << " OK" << std::endl;

  std::cerr << "Training Exon durations..." << std::flush;
  exon_single_duration = trainDuration("exon_single", 15000, 10, 0.2);
  exon_initial_duration = trainDuration("exon_initial", 15000, 10, 0.2);
  exon_internal_duration = trainDuration("exon_internal", 15000, 10, 0.2);
  exon_terminal_duration = trainDuration("exon_final", 15000, 10, 0.2);
  std::cerr << " OK" << std::endl;

  // Donors
  std::cerr << "Training Donor models..." << std::flush;
  donor_model = trainDonorModel(nucleotides);
  rdonor_model = trainRDonorModel(nucleotides);
  std::cerr << " OK" << std::endl;

  // Intron
  std::cerr << "Training Short duration..." << std::flush;
  short_duration = trainDuration("intron_short", 585, 2, 0.4);
  std::cerr << " OK" << std::endl;

  // Acceptors
  std::cerr << "Training Acceptor models..." << std::flush;
  acceptor_model = trainAcceptorModel(nucleotides);
  racceptor_model = trainRAcceptorModel(nucleotides);
  std::cerr << " OK" << std::endl;

  // Stop
  std::cerr << "Training Stop models..." << std::flush;
  stop_model = trainStopModel(nucleotides);
  rstop_model = trainRStopModel(nucleotides);
  std::cerr << " OK" << std::endl << std::endl;
}

/*----------------------------------------------------------------------------*/

}  // namespace myop
}  // namespace tops
