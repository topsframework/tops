#include "helper/VariableLengthMarkovChain.hpp"

#include "Random.hpp"

namespace tops {
namespace helper {

tops::model::VariableLengthMarkovChainPtr generateRandomVLMC(int number_of_nodes, int alphabet_size) {
  auto alphabet = tops::AlphabetPtr(new tops::Alphabet());
  for (int i = 0; i < alphabet_size; i++)
    alphabet->createSymbol(std::to_string(i));
  std::vector<double> counts(alphabet_size, 13.0);
  auto iid = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(counts));
  auto tree = tops::ContextTreePtr(new tops::ContextTree(alphabet));
  auto node = tree->createContext();
  node->setDistribution(iid);
  for (int i = 1; i < number_of_nodes; i++) {
    auto _node = tree->createContext();
    _node->setDistribution(iid);
    node->setChild(_node, generateRandomInteger(alphabet_size-1));
    node = _node;
  }
  return tops::model::VariableLengthMarkovChain::make(tree);
}

tops::model::VariableLengthMarkovChainPtr createMachlerVLMC() {
  auto root = tops::model::ContextTreeNode::make(0, tops::model::DiscreteIIDModel::make({log(0.50), log(0.50)}));
  auto c0 = root->addChild(0, tops::model::DiscreteIIDModel::make({log(0.20), log(0.80)}));
  auto c1 = root->addChild(1, tops::model::DiscreteIIDModel::make({log(0.21), log(0.79)}));
  auto c10 = c1->addChild(0, tops::model::DiscreteIIDModel::make({log(0.22), log(0.78)}));
  auto c11 = c1->addChild(1, tops::model::DiscreteIIDModel::make({log(0.25), log(0.75)}));
  auto c100 = c10->addChild(0, tops::model::DiscreteIIDModel::make({log(0.30), log(0.70)}));
  auto c101 = c10->addChild(1, tops::model::DiscreteIIDModel::make({log(0.10), log(0.90)}));
  auto tree = tops::model::ContextTree::make(root);
  return tops::model::VariableLengthMarkovChain::make(tree);
}

}  // namespace helper
}  // namespace tops
