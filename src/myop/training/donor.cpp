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
#include <dirent.h>

// Internal headers
#include "myop/Alphabet.hpp"

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
namespace myop {

/*----------------------------------------------------------------------------*/

ProbabilisticModelPtr trainDonorModel(AlphabetPtr nucleotides) {
  DIR *dir;
  struct dirent *ent;

  // Donor

  std::vector<Sequence> dataset1;
  std::vector<double> weights1;
  if ((dir = opendir((DATASET + "/donor").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset1.push_back(readSequence(DATASET + "/donor/" + filename, {nucleotides})[0]);
        weights1.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 4, 9, 0, 0, 1.0, Sequence{2, 3}, 3, weights1);

  for (auto s : dataset1)
    trainer->add_training_sequence(s);

  auto donor = trainer->train();

  // Initial pattern

  std::vector<Sequence> dataset2;
  std::vector<double> weights2;

  if ((dir = opendir((DATASET + "/donor_initial_pattern").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset2.push_back(readSequence(DATASET + "/donor_initial_pattern/" + filename, {nucleotides})[0]);
        weights2.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer2
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 3, 4, 0, 0, 100.0, Sequence{}, 0, weights2);

  for (auto s : dataset2)
    trainer2->add_training_sequence(s);

  auto initial_pattern = trainer2->train();

  // Multiple Sequential Model

  return MultipleSequentialModel::make(
      std::vector<ProbabilisticModelPtr>{ initial_pattern, donor },
      std::vector<int>{4, 9});
}

/*----------------------------------------------------------------------------*/

ProbabilisticModelPtr trainRDonorModel(AlphabetPtr nucleotides) {
  DIR *dir;
  struct dirent *ent;

  // Donor

  std::vector<Sequence> dataset1;
  std::vector<double> weights1;
  if ((dir = opendir((DATASET + "/rdonor").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset1.push_back(readSequence(DATASET + "/rdonor/" + filename, {nucleotides})[0]);
        weights1.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 4, 9, 0, 0, 1.0, Sequence{3, 0}, 3, weights1);

  for (auto s : dataset1)
    trainer->add_training_sequence(s);

  auto donor = trainer->train();

  // Initial pattern

  std::vector<Sequence> dataset2;
  std::vector<double> weights2;
  if ((dir = opendir((DATASET + "/rdonor_initial_pattern").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset2.push_back(readSequence(DATASET + "/rdonor_initial_pattern/" + filename, {nucleotides})[0]);
        weights2.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer2
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 3, 4, 0, 0, 100.0, Sequence{}, 0, weights2);

  for (auto s : dataset2)
    trainer2->add_training_sequence(s);

  auto initial_pattern = trainer2->train();

  // Multiple Sequential Model

  return MultipleSequentialModel::make(
      std::vector<ProbabilisticModelPtr>{ donor, initial_pattern },
      std::vector<int>{9, 4});
}

/*----------------------------------------------------------------------------*/

}  // namespace myop
}  // namespace model
