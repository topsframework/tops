#include "VLMC.hpp"

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

}  // namespace helper
}  // namespace tops
