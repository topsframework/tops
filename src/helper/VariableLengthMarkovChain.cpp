#include "helper/VariableLengthMarkovChain.hpp"

#include <cmath>

#include "helper/Random.hpp"
#include "helper/DiscreteIIDModel.hpp"

namespace tops {
namespace helper {

tops::model::VariableLengthMarkovChainPtr generateRandomVLMC(int number_of_nodes, int alphabet_size) {
  auto node = tops::model::ContextTreeNode::make(generateRandomIIDModel(alphabet_size));
  auto root = node;
  for (int i = 1; i < number_of_nodes; i++) {
    for (int j = 0; j < alphabet_size; j++) {
      node->addChild(j, generateRandomIIDModel(alphabet_size));
    }
  }
  return tops::model::VariableLengthMarkovChain::make(tops::model::ContextTree::make(root));
}

tops::model::VariableLengthMarkovChainPtr createMachlerVLMC() {
  auto root = tops::model::ContextTreeNode::make(tops::model::DiscreteIIDModel::make({log(0.50), log(0.50)}));
  auto c0 = root->addChild(0, tops::model::DiscreteIIDModel::make({log(0.20), log(0.80)}));
  auto c1 = root->addChild(1, tops::model::DiscreteIIDModel::make({log(0.21), log(0.79)}));
  auto c10 = c1->addChild(0, tops::model::DiscreteIIDModel::make({log(0.22), log(0.78)}));
  auto c11 = c1->addChild(1, tops::model::DiscreteIIDModel::make({log(0.25), log(0.75)}));
  auto c100 = c10->addChild(0, tops::model::DiscreteIIDModel::make({log(0.30), log(0.70)}));
  auto c101 = c10->addChild(1, tops::model::DiscreteIIDModel::make({log(0.10), log(0.90)}));
  return tops::model::VariableLengthMarkovChain::make(tops::model::ContextTree::make(root));
}

tops::model::VariableLengthMarkovChainPtr createVLMCMC() {
  auto root = tops::model::ContextTreeNode::make(tops::model::DiscreteIIDModel::make({log(0.50), log(0.50)}));
  root->addChild(0, tops::model::DiscreteIIDModel::make({log(0.10), log(0.90)}));
  return tops::model::VariableLengthMarkovChain::make(tops::model::ContextTree::make(root));
}

}  // namespace helper
}  // namespace tops
