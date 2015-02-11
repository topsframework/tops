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
#include <cmath>

// ToPS headers
#include "helper/VariableLengthMarkovChain.hpp"
#include "helper/Random.hpp"
#include "helper/DiscreteIIDModel.hpp"

namespace tops {
namespace helper {

tops::model::VariableLengthMarkovChainPtr generateRandomVLMC(
    int number_of_nodes,
    int alphabet_size) {
  auto tree = tops::model::ContextTree::make(alphabet_size);
  auto node = tree->createContext();
  node->setDistribution(generateRandomIIDModel(alphabet_size));
  for (int i = 1; i < number_of_nodes; i++) {
    auto root = node;
    for (int j = 0; j < alphabet_size; j++) {
      node = tree->createContext();
      node->setDistribution(generateRandomIIDModel(alphabet_size));
      node->setChild(root, j);
    }
  }
  return tops::model::VariableLengthMarkovChain::make(tree);
}

tops::model::VariableLengthMarkovChainPtr createMachlerVLMC() {
  auto tree = tops::model::ContextTree::make(2);
  auto root = tree->createContext();
  root->setDistribution(
    tops::model::DiscreteIIDModel::make({log(0.50), log(0.50)}));
  auto c0 = tree->createContext();
  root->setChild(c0, 0);
  c0->setDistribution(
    tops::model::DiscreteIIDModel::make({log(0.20), log(0.80)}));
  auto c1 = tree->createContext();
  root->setChild(c1, 1);
  c1->setDistribution(
    tops::model::DiscreteIIDModel::make({log(0.21), log(0.79)}));
  auto c10 = tree->createContext();
  c1->setChild(c10, 0);
  c10->setDistribution(
    tops::model::DiscreteIIDModel::make({log(0.22), log(0.78)}));
  auto c11 = tree->createContext();
  c1->setChild(c11, 1);
  c11->setDistribution(
    tops::model::DiscreteIIDModel::make({log(0.25), log(0.75)}));
  auto c100 = tree->createContext();
  c10->setChild(c100, 0);
  c100->setDistribution(
    tops::model::DiscreteIIDModel::make({log(0.30), log(0.70)}));
  auto c101 = tree->createContext();
  c10->setChild(c101, 1);
  c101->setDistribution(
    tops::model::DiscreteIIDModel::make({log(0.10), log(0.90)}));
  return tops::model::VariableLengthMarkovChain::make(tree);
}

tops::model::VariableLengthMarkovChainPtr createVLMCMC() {
  auto tree = tops::model::ContextTree::make(2);
  auto root = tree->createContext();
  root->setDistribution(
    tops::model::DiscreteIIDModel::make({log(0.50), log(0.50)}));
  auto c0 = tree->createContext();
  root->setChild(c0, 0);
  c0->setDistribution(
    tops::model::DiscreteIIDModel::make({log(0.10), log(0.90)}));
  return tops::model::VariableLengthMarkovChain::make(tree);
}

}  // namespace helper
}  // namespace tops
