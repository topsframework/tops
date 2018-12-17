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

// Standard headers
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>

// Internal headers
#include "myop/Myop.hpp"
#include "myop/states.hpp"
#include "model/Sequence.hpp"
#include "hints.cpp"


namespace tops {
namespace myop {
  extern tops::model::Sequence sequence;
  extern tops::model::Sequence conservation_sequence;
}
}

using namespace tops::myop;

int main(int argc, char *argv[]) {
  string datasetFolder = "dataset";
  string sequenceFile;
  string conservationSequenceFile;
  string technique;

  for (int i = 0; i < argc; i++) {
    std::string s(argv[i]);
    if (s == "-d") {
      datasetFolder = argv[i+1];
    }
    else if (s == "-s") {
      sequenceFile = argv[i+1];
    }
    else if (s == "-c") {
          conservationSequenceFile = argv[i+1];
    }
    else if (s == "-t") {
      technique = argv[i+1];
    }
  }

  cout << "datasetFolder: " << datasetFolder << endl;
  cout << "sequenceFile: " << sequenceFile << endl;
  cout << "conservationSequenceFile: " << conservationSequenceFile << endl;
  cout << "technique: " << technique << endl;

  std::shared_ptr<FastaConverter> fc = std::make_shared<FastaConverter>();
  auto fastaSequences = fc->converteFastaFileToFastaSequences(sequenceFile);
  sequence = fc->convertFastaSequenceToToPSSequence(fastaSequences[0]);
  ExtrinsicTechniquePtr extrinsic_technique;

  if (technique == "augustus") {
      extrinsic_technique = std::make_shared<Augustus>(69, sequence.size());
  } else if (technique == "noHints") {
      extrinsic_technique = std::make_shared<NoHints>(69, sequence.size());
  } else if (technique == "twinscan") {
    auto fastaConservationSequeces =
      fc->converteFastaFileToFastaSequences(conservationSequenceFile);
    conservation_sequence = fc->convertFastaConservationSequenceToToPSSequence(
                                                  fastaConservationSequeces[0]);
    extrinsic_technique = std::make_shared<Twinscan>(69, sequence.size());
  }

  tops::myop::Myop myop(datasetFolder);

  auto[ estimation, prediction ]
    = myop.predict(sequence, extrinsic_technique);

  string header = "<" + fastaSequences[0].getSequenceName() + ":1," +
                    std::to_string(sequence.size()) + ">," +
                    fastaSequences[0].getSequenceName() + ": " +
                    std::to_string(estimation) + ":\t";
  string sequence_path;
  for (size_t i = 1; i < prediction.size()-1; i++) {
    sequence_path += tops::myop::state_names[prediction[i]] + " ";
  }

  std::cout << header + sequence_path << std::endl;
  return 0;
}
