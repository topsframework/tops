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
//#include "model/Matrix.hpp"


int main(int argc, char *argv[]) {
  std::string dataset = "dataset";

  if (argc >= 2) {
    dataset = argv[1];
  }

  // Random sequence
  // 1, 3, 1, 2, 0, 1, 3, 0, 2

  // Real gene (AT1G34280)
  tops::model::Sequence sequence {
    //ATAATAACTTG A 0, C 1, G 2, T 3
    0, 3, 0, 0, 3, 0, 0, 1, 3, 3, 2, 2, 0, 2, 0, 0, 0, 3, 3, 2, 0, 3, 
    3, 1, 3, 0, 1, 2, 3, 0, 0, 3, 2, 3, 0, 2, 0, 2, 2, 0, 0, 2, 0, 0,
    3, 0, 2, 0, 1, 0, 1, 0, 3, 2, 0, 3, 3, 3, 3, 1, 0, 3, 0, 2, 3, 3,
    0, 3, 0, 0, 3, 3, 0, 3, 1, 1, 1, 0, 3, 2, 3, 2, 1, 3, 3, 0, 3, 0,
    3, 3, 1, 0, 0, 0, 1, 1, 3, 3, 2, 0, 3, 1, 0, 1, 1, 0, 1, 1, 0, 0,
    0, 1, 1, 1, 0, 1, 1, 1, 3, 2, 0, 1, 3, 2, 0, 3, 1, 1, 0, 0, 2, 0,
    1, 1, 0, 0, 1, 0, 2, 0, 1, 1, 0, 3, 1, 1, 0, 0, 0, 2, 3, 1, 0, 0,
    3, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 3, 0, 2, 2, 1, 1, 1, 3, 1, 1, 0,
    1, 3, 0, 2, 3, 0, 2, 0, 3, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0,
    0, 3, 2, 1, 3, 3, 3, 0, 1, 1, 3, 0, 0, 0, 2, 0, 1, 3, 3, 0, 3, 3,
    0, 1, 1, 0, 1, 1, 0, 3, 3, 3, 1, 1, 0, 2, 0, 0, 0, 1, 0, 3, 1, 0,
    2, 0, 3, 3, 3, 3, 0, 1, 0, 3, 0, 3, 3, 1, 0, 1, 3, 3, 1, 1, 0, 1,
    3, 0, 3, 3, 2, 0, 3, 2, 1, 1, 0, 2, 3, 3, 0, 1, 3, 1, 2, 3, 0, 0,
    2, 3, 3, 2, 0, 3, 1, 0, 3, 0, 3, 1, 0, 1, 0, 0, 2, 2, 0, 3, 3, 1,
    0, 3, 0, 3, 0, 3, 3, 3, 0, 3, 2, 0, 0, 1, 0, 1, 1, 2, 2, 0, 3, 0,
    0, 1, 1, 2, 1, 3, 1, 1, 0, 3, 3, 3, 0, 3, 1, 2, 0, 0, 3, 3, 3, 3,
    0, 3, 3, 1, 3, 1, 1, 3, 1, 0, 1, 3, 3, 1, 2, 3, 0, 0, 1, 3, 3, 3,
    0, 3, 0, 0, 0, 3, 1, 1, 2, 3, 1, 0, 1, 0, 0, 0, 3, 0, 3, 1, 3, 3,
    1, 0, 3, 1, 2, 3, 3, 1, 0, 3, 3, 0, 2, 1, 0, 0, 1, 3, 3, 2, 3, 3,
    3, 2, 1, 3, 3, 1, 1, 1, 0, 3, 1, 0, 0, 0, 0, 3, 0, 3, 0, 3, 3, 1,
  };

  ExtrinsicTechniquePtr extrinsic_technique;

  if (argc >= 3) {
    string extrinsicMethod = argv[2];
    if(extrinsicMethod.compare("augustus") == 0){
      extrinsic_technique
      = std::make_shared<Augustus>(69, sequence.size());
    } else if (extrinsicMethod.compare("noHints") == 0) {
      extrinsic_technique
        = std::make_shared<NoHints>(69, sequence.size());
    } else if (extrinsicMethod.compare("twinscan") == 0) {
      extrinsic_technique
        = std::make_shared<Twinscan>(69, sequence.size());
    }
  } else if (argc == 1) {
    extrinsic_technique
        = std::make_shared<NoHints>(69, sequence.size());
  }

  tops::myop::Myop myop(dataset);

  auto [ estimation, prediction ]
    = myop.predict(sequence, extrinsic_technique);

  string header = "<sequence_name:1," + std::to_string(sequence.size()) + ">,sequence_name: " + std::to_string(estimation) + ":\t";
  string sequence_path;
  for (size_t i = 1; i < prediction.size()-1; i++) {
    sequence_path += tops::myop::state_names[prediction[i]] + " ";
  }

  std::cout << std::endl;
  return 0;
}
