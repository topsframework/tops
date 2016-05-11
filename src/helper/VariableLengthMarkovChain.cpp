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
#include "helper/VariableLengthMarkovChain.hpp"

// Standard headers
#include <cmath>
#include <vector>

// ToPS headers
#include "helper/Random.hpp"
#include "helper/DiscreteIIDModel.hpp"

#include "model/Probability.hpp"

namespace tops {
namespace helper {

/*----------------------------------------------------------------------------*/
/*                                 FUNCTIONS                                  */
/*----------------------------------------------------------------------------*/

model::VariableLengthMarkovChainPtr generateRandomVLMC(
    unsigned int number_of_nodes,
    unsigned int alphabet_size) {
  auto tree = model::ContextTree::make(alphabet_size);
  auto node = tree->createContext();
  node->setDistribution(generateRandomIIDModel(alphabet_size));
  for (unsigned int i = 1; i < number_of_nodes; i++) {
    auto root = node;
    for (unsigned int j = 0; j < alphabet_size; j++) {
      node = tree->createContext();
      node->setDistribution(generateRandomIIDModel(alphabet_size));
      node->setChild(root, j);
    }
  }
  return model::VariableLengthMarkovChain::make(tree);
}

/*----------------------------------------------------------------------------*/

model::VariableLengthMarkovChainPtr createVLMCMC() {
  auto tree = model::ContextTree::make(2);

  auto root = tree->createContext();
  root->setDistribution(model::DiscreteIIDModel::make(
    std::vector<model::Probability>{ log(0.50), log(0.50) }));

  auto c0 = tree->createContext();
  root->setChild(c0, 0);
  c0->setDistribution(model::DiscreteIIDModel::make(
    std::vector<model::Probability>{ log(0.10), log(0.90) }));

  return model::VariableLengthMarkovChain::make(tree);
}

/*----------------------------------------------------------------------------*/

model::VariableLengthMarkovChainPtr createMachlerVLMC() {
  auto tree = model::ContextTree::make(2);

  auto root = tree->createContext();
  root->setDistribution(model::DiscreteIIDModel::make(
    std::vector<model::Probability>{ log(0.50), log(0.50) }));

  auto c0 = tree->createContext();
  root->setChild(c0, 0);
  c0->setDistribution(model::DiscreteIIDModel::make(
    std::vector<model::Probability>{ log(0.20), log(0.80) }));

  auto c1 = tree->createContext();
  root->setChild(c1, 1);
  c1->setDistribution(model::DiscreteIIDModel::make(
    std::vector<model::Probability>{ log(0.21), log(0.79) }));

  auto c10 = tree->createContext();
  c1->setChild(c10, 0);
  c10->setDistribution(model::DiscreteIIDModel::make(
    std::vector<model::Probability>{ log(0.22), log(0.78) }));

  auto c11 = tree->createContext();
  c1->setChild(c11, 1);
  c11->setDistribution(model::DiscreteIIDModel::make(
    std::vector<model::Probability>{ log(0.25), log(0.75) }));

  auto c100 = tree->createContext();
  c10->setChild(c100, 0);
  c100->setDistribution(model::DiscreteIIDModel::make(
    std::vector<model::Probability>{ log(0.30), log(0.70) }));

  auto c101 = tree->createContext();
  c10->setChild(c101, 1);
  c101->setDistribution(model::DiscreteIIDModel::make(
    std::vector<model::Probability>{ log(0.10), log(0.90) }));

  return model::VariableLengthMarkovChain::make(tree);
}

/*----------------------------------------------------------------------------*/

}  // namespace helper
}  // namespace tops
