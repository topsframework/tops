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
#include "myop/training.hpp"

// Standard headers
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <iostream>

// Internal headers
#include "myop/Alphabet.hpp"
#include "myop/IntegerAlphabet.hpp"

#include "model/Symbol.hpp"

#include "model/DiscreteIIDModel.hpp"

// Imports
using tops::model::Symbol;

using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;

namespace tops {
namespace myop {

/*----------------------------------------------------------------------------*/

std::vector<std::vector<Symbol>>
readSequence(std::string filename, std::vector<AlphabetPtr> alphabets) {
  std::vector<std::vector<Symbol>> sequence;
  std::ifstream file(filename);
  std::string line;
  bool first = true;
  while (std::getline(file, line)) {
    line += "\n";
    if (first) {
      int prev = -1;
      int i = 0;
      std::size_t found = line.find_first_of(" \t\n");
      while (found != std::string::npos) {
        if (found - prev - 1 > 0) {
          sequence.push_back(std::vector<Symbol>{});
          i++;
        }
        prev = found;
        found = line.find_first_of(" \t\n", found + 1);
      }
      first = false;
    } else {
      int prev = -1;
      int i = 0;
      std::size_t found = line.find_first_of(" \t\n");
      while (found != std::string::npos) {
        if (found - prev - 1 > 0) {
          std::string sym = line.substr(prev + 1, found - prev - 1);
          sequence[i].push_back(alphabets[i]->getID(sym));
          i++;
        }
        prev = found;
        found = line.find_first_of(" \t\n", found + 1);
      }
    }
  }
  return sequence;
}

/*----------------------------------------------------------------------------*/

DiscreteIIDModelPtr trainDuration(std::string model_name,
                                  unsigned int max,
                                  unsigned int m,
                                  double slope) {
  auto trainer
    = DiscreteIIDModel::standardTrainer(
        DiscreteIIDModel::smoothed_histogram_stanke_algorithm{},
        std::vector<unsigned int>{1}, max, m, slope);

  std::string file = DATASET + "/" + model_name + "_duration/sequence.0.tsv";
  std::vector<AlphabetPtr> alphabets { std::make_shared<IntegerAlphabet>() };

  trainer->add_training_sequence(readSequence(file, alphabets)[0]);

  return trainer->train();
}

/*----------------------------------------------------------------------------*/

}  // namespace myop
}  // namespace model
