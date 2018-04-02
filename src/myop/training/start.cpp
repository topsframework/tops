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
#include "model/Probability.hpp"

#include "model/ContextTree.hpp"
#include "model/DiscreteIIDModel.hpp"
#include "model/ProbabilisticModel.hpp"
#include "model/MultipleSequentialModel.hpp"
#include "model/InhomogeneousMarkovChain.hpp"
#include "model/VariableLengthMarkovChain.hpp"

// Imports
using tops::model::Sequence;
using tops::model::Probability;

using tops::model::ContextTree;
using tops::model::DiscreteIIDModel;
using tops::model::ProbabilisticModel;
using tops::model::MultipleSequentialModel;
using tops::model::InhomogeneousMarkovChain;
using tops::model::VariableLengthMarkovChain;

using tops::model::ContextTreePtr;
using tops::model::DiscreteIIDModelPtr;
using tops::model::ProbabilisticModelPtr;
using tops::model::MultipleSequentialModelPtr;
using tops::model::InhomogeneousMarkovChainPtr;
using tops::model::VariableLengthMarkovChainPtr;

namespace tops {
namespace myop {

/*----------------------------------------------------------------------------*/

ProbabilisticModelPtr trainStartModel(AlphabetPtr nucleotides) {
  DIR *dir;
  struct dirent *ent;

  // Initiation motif

  std::vector<Sequence> dataset1;
  std::vector<double> weights1;
  if ((dir = opendir((DATASET + "/start_initiation_motif").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset1.push_back(readSequence(DATASET + "/start_initiation_motif/" + filename, {nucleotides})[0]);
        weights1.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 1, 20, 0, 0, 1.0, Sequence{}, 0, weights1);

  for (auto s : dataset1)
    trainer->add_training_sequence(s);

  auto initiation_motif = trainer->train();

  // ATG

  auto a_tree = ContextTree::make(4);
  auto a_root = a_tree->createContext();
  a_root->setDistribution(DiscreteIIDModel::make(
        std::vector<Probability>{{ 1.0, 0.0, 0.0, 0.0 }}));

  auto t_tree = ContextTree::make(4);
  auto t_root = t_tree->createContext();
  t_root->setDistribution(DiscreteIIDModel::make(
        std::vector<Probability>{{ 0.0, 0.0, 0.0, 1.0 }}));

  auto g_tree = ContextTree::make(4);
  auto g_root = g_tree->createContext();
  g_root->setDistribution(DiscreteIIDModel::make(
        std::vector<Probability>{{ 0.0, 0.0, 1.0, 0.0 }}));

  auto atg = InhomogeneousMarkovChain::make(
    std::vector<VariableLengthMarkovChainPtr>{
      VariableLengthMarkovChain::make(a_tree),
      VariableLengthMarkovChain::make(t_tree),
      VariableLengthMarkovChain::make(g_tree)
  });

  // Initial pattern

  std::vector<Sequence> dataset2;
  std::vector<double> weights2;
  if ((dir = opendir((DATASET + "/start_initial_pattern").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset2.push_back(readSequence(DATASET + "/start_initial_pattern/" + filename, {nucleotides})[0]);
        weights2.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer2
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 3, 4, 0, 0, 1.0, Sequence{}, 0, weights2);

  for (auto s : dataset2)
    trainer2->add_training_sequence(s);

  auto initial_pattern = trainer2->train();

  // Multiple Sequential Model

  return MultipleSequentialModel::make(
      std::vector<ProbabilisticModelPtr>{
        initiation_motif, atg, initial_pattern },
      std::vector<int>{20, 3, 4});
}

/*----------------------------------------------------------------------------*/

ProbabilisticModelPtr trainRStartModel(AlphabetPtr nucleotides) {
  DIR *dir;
  struct dirent *ent;

  // Initiation motif

  std::vector<Sequence> dataset1;
  std::vector<double> weights1;
  if ((dir = opendir((DATASET + "/rstart_initiation_motif").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset1.push_back(readSequence(DATASET + "/rstart_initiation_motif/" + filename, {nucleotides})[0]);
        weights1.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer1
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 1, 20, 0, 0, 1.0, Sequence{}, 0, weights1);

  for (auto s : dataset1)
    trainer1->add_training_sequence(s);

  auto initiation_motif = trainer1->train();

  // ATG

  auto a_tree = ContextTree::make(4);
  auto a_root = a_tree->createContext();
  a_root->setDistribution(DiscreteIIDModel::make(
        std::vector<Probability>{{ 0.0, 1.0, 0.0, 0.0 }}));

  auto t_tree = ContextTree::make(4);
  auto t_root = t_tree->createContext();
  t_root->setDistribution(DiscreteIIDModel::make(
        std::vector<Probability>{{ 1.0, 0.0, 0.0, 0.0 }}));

  auto g_tree = ContextTree::make(4);
  auto g_root = g_tree->createContext();
  g_root->setDistribution(DiscreteIIDModel::make(
        std::vector<Probability>{{ 0.0, 0.0, 0.0, 1.0 }}));

  auto atg = InhomogeneousMarkovChain::make(
    std::vector<VariableLengthMarkovChainPtr>{
      VariableLengthMarkovChain::make(a_tree),
      VariableLengthMarkovChain::make(t_tree),
      VariableLengthMarkovChain::make(g_tree)
  });

  // Initial pattern

  std::vector<Sequence> dataset2;
  std::vector<double> weights2;
  if ((dir = opendir((DATASET + "/rstart_initial_pattern").c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      auto filename = std::string(ent->d_name);
      if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
        dataset2.push_back(readSequence(DATASET + "/rstart_initial_pattern/" + filename, {nucleotides})[0]);
        weights2.push_back(1.0);
      }
    }
    closedir(dir);
  }

  auto trainer2
    = InhomogeneousMarkovChain::standardTrainer(
        InhomogeneousMarkovChain::wam_algorithm{},
        4, 3, 4, 0, 0, 1.0, Sequence{}, 0, weights2);

  for (auto s : dataset2)
    trainer2->add_training_sequence(s);

  auto initial_pattern = trainer2->train();

  // Multiple Sequential Model

  return MultipleSequentialModel::make(
      std::vector<ProbabilisticModelPtr>{
        initial_pattern, atg, initiation_motif },
      std::vector<int>{4, 3, 20});
}

/*----------------------------------------------------------------------------*/

}  // namespace myop
}  // namespace model
