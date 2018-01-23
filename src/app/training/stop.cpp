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
#include "app/training.hpp"

// Standard headers
#include <memory>
#include <string>
#include <vector>
#include <dirent.h>

// Internal headers
#include "app/Alphabet.hpp"

#include "model/Sequence.hpp"

#include "model/ProbabilisticModel.hpp"
#include "model/MultipleSequentialModel.hpp"
#include "model/InhomogeneousMarkovChain.hpp"

// Imports
using tops::model::Sequence;

using tops::model::ProbabilisticModel;
using tops::model::MultipleSequentialModel;
using tops::model::InhomogeneousMarkovChain;

using tops::model::ProbabilisticModelPtr;
using tops::model::MultipleSequentialModelPtr;
using tops::model::InhomogeneousMarkovChainPtr;

namespace tops {
namespace app {

/*----------------------------------------------------------------------------*/

ProbabilisticModelPtr trainStopModel(AlphabetPtr nucleotides) {
  DIR *dir;
  struct dirent *ent;

  std::vector<Sequence> dataset;
  std::vector<double> weights;
  if ((dir = opendir((DATASET + "/stop").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset.push_back(readSequence(DATASET + "/stop/" + filename, {nucleotides})[0]);
        weights.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 2, 3, 0, 0, 0.0, Sequence{}, 0, weights);

  for (auto s : dataset)
    trainer->add_training_sequence(s);

  return trainer->train();
}

/*----------------------------------------------------------------------------*/

ProbabilisticModelPtr trainRStopModel(AlphabetPtr nucleotides) {
  DIR *dir;
  struct dirent *ent;

  std::vector<Sequence> dataset;
  std::vector<double> weights;
  if ((dir = opendir((DATASET + "/rstop").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset.push_back(readSequence(DATASET + "/rstop/" + filename, {nucleotides})[0]);
        weights.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 2, 3, 0, 0, 0.0, Sequence{}, 0, weights);

  for (auto s : dataset)
    trainer->add_training_sequence(s);

  return trainer->train();
}

/*----------------------------------------------------------------------------*/

}  // namespace app
}  // namespace model
