#include "helper/InhomogeneousMarkovChain.hpp"

#include "helper/VariableLengthMarkovChain.hpp"

namespace tops {
namespace helper {

tops::model::InhomogeneousMarkovChainPtr generateRandomIMC(int number_of_chains, int alphabet_size) {
  std::vector<tops::model::VariableLengthMarkovChainPtr> vlmcs;
  for (int i = 0; i < number_of_chains; i++)
    vlmcs.push_back(generateRandomVLMC(number_of_chains, alphabet_size));
  return tops::model::InhomogeneousMarkovChain::make(vlmcs, true);
}

}  // namespace helper
}  // namespace tops
