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

ProbabilisticModelPtr trainAcceptorModel(AlphabetPtr nucleotides) {
  DIR *dir;
  struct dirent *ent;

  // Branch

  std::vector<Sequence> dataset1;
  std::vector<double> weights1;
  if ((dir = opendir((DATASET + "/acceptor_branch").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset1.push_back(readSequence(DATASET + "/acceptor_branch/" + filename, {nucleotides})[0]);
        weights1.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 3, 32, 0, 3, 1.0, Sequence{}, 0, weights1);

  for (auto s : dataset1)
    trainer->add_training_sequence(s);

  auto branch = trainer->train();

  // Acceptor

  std::vector<Sequence> dataset2;
  std::vector<double> weights2;
  if ((dir = opendir((DATASET + "/acceptor").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset2.push_back(readSequence(DATASET + "/acceptor/" + filename, {nucleotides})[0]);
        weights2.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer2
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 5, 6, 0, 0, 1.0, Sequence{0, 2}, 3, weights2);

  for (auto s : dataset2)
    trainer2->add_training_sequence(s);

  auto acceptor = trainer2->train();

  // Initial pattern

  std::vector<Sequence> dataset3;
  std::vector<double> weights3;
  if ((dir = opendir((DATASET + "/acceptor_initial_pattern").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset3.push_back(readSequence(DATASET + "/acceptor_initial_pattern/" + filename, {nucleotides})[0]);
        weights3.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer3
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 3, 4, 0, 0, 100.0, Sequence{}, 0, weights3);

  for (auto s : dataset3)
    trainer3->add_training_sequence(s);

  auto initial_pattern = trainer3->train();

  // Multiple Sequential Model

  return MultipleSequentialModel::make(
      std::vector<ProbabilisticModelPtr>{ branch, acceptor, initial_pattern },
      std::vector<int>{32, 6, 4});
}

/*----------------------------------------------------------------------------*/

ProbabilisticModelPtr trainRAcceptorModel(AlphabetPtr nucleotides) {
  DIR *dir;
  struct dirent *ent;

  // Branch

  std::vector<Sequence> dataset1;
  std::vector<double> weights1;
  if ((dir = opendir((DATASET + "/racceptor_branch").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset1.push_back(readSequence(DATASET + "/racceptor_branch/" + filename, {nucleotides})[0]);
        weights1.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 3, 32, 0, 3, 1.0, Sequence{}, 0, weights1);

  for (auto s : dataset1)
    trainer->add_training_sequence(s);

  auto branch = trainer->train();

  // Acceptor

  std::vector<Sequence> dataset2;
  std::vector<double> weights2;
  if ((dir = opendir((DATASET + "/racceptor").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset2.push_back(readSequence(DATASET + "/racceptor/" + filename, {nucleotides})[0]);
        weights2.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer2
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 5, 6, 0, 0, 1.0, Sequence{1, 3}, 1, weights2);

  for (auto s : dataset2)
    trainer2->add_training_sequence(s);

  auto acceptor = trainer2->train();

  // Initial pattern

  std::vector<Sequence> dataset3;
  std::vector<double> weights3;
  if ((dir = opendir((DATASET + "/racceptor_initial_pattern").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset3.push_back(readSequence(DATASET + "/racceptor_initial_pattern/" + filename, {nucleotides})[0]);
        weights3.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer3
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 3, 4, 0, 0, 100.0, Sequence{}, 0, weights3);

  for (auto s : dataset3)
    trainer3->add_training_sequence(s);

  auto initial_pattern = trainer3->train();

  // Multiple Sequential Model

  return MultipleSequentialModel::make(
      std::vector<ProbabilisticModelPtr>{ initial_pattern, acceptor, branch },
      std::vector<int>{4, 6, 32});
}

/*----------------------------------------------------------------------------*/

}  // namespace myop
}  // namespace model
