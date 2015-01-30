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
  auto alphabet = tops::AlphabetPtr(new tops::Alphabet());
  alphabet->createSymbol("0");
  alphabet->createSymbol("1");
  auto context_tree = tops::ContextTreePtr(new tops::ContextTree(alphabet));
  auto c0 = context_tree->createContext();
  auto p0 = {0.5, 0.5};
  auto iid0 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p0));
  c0->setDistribution(iid0);
  auto c1 = context_tree->createContext();
  auto p1 = {0.6, 0.4};
  auto iid1 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p1));
  c1->setDistribution(iid1);
  auto c2 = context_tree->createContext();
  auto p2 = {0.3, 0.7};
  auto iid2 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p2));
  c2->setDistribution(iid2);
  auto c3 = context_tree->createContext();
  auto p3 = {0.7, 0.3};
  auto iid3 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p3));
  c3->setDistribution(iid3);
  auto c4 = context_tree->createContext();
  auto p4 = {0.1, 0.9};
  auto iid4 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p4));
  c4->setDistribution(iid4);
  auto c5 = context_tree->createContext();
  auto p5 = {0.3, 0.7};
  auto iid5 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p5));
  c5->setDistribution(iid5);
  auto c6 = context_tree->createContext();
  auto p6 = {0.7, 0.3};
  auto iid6 = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(p6));
  c6->setDistribution(iid6);

  c0->setChild(c1, 0);
  c0->setChild(c2, 1);
  c2->setChild(c3, 0);
  c2->setChild(c4, 1);
  c3->setChild(c5, 0);
  c3->setChild(c6, 1);

  return tops::model::VariableLengthMarkovChain::make(context_tree);
}

}  // namespace helper
}  // namespace tops
