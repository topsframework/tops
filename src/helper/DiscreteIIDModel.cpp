#include "helper/DiscreteIIDModel.hpp"

#include <cmath>

#include "helper/Random.hpp"

namespace tops {
namespace helper {

tops::model::DiscreteIIDModelPtr generateRandomIIDModel(int alphabet_size) {
  std::vector<double> counts;
  for (int i = 0; i < alphabet_size; i++)
    counts.push_back(static_cast<double>(generateRandomInteger(alphabet_size)));
  return tops::model::DiscreteIIDModel::make(counts);
}

tops::model::DiscreteIIDModelPtr createFairCoinIIDModel() {
  return tops::model::DiscreteIIDModel::make({log(0.5), log(0.5)});
}

tops::model::DiscreteIIDModelPtr createLoadedCoinIIDModel() {
  return tops::model::DiscreteIIDModel::make({log(0.2), log(0.8)});
}

}  // namespace helper
}  // namespace tops
