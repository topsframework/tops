#include "helper/DiscreteIIDModel.hpp"

#include "helper/Random.hpp"

namespace tops {
namespace helper {

tops::model::DiscreteIIDModelPtr generateRandomIIDModel(int alphabet_size) {
  std::vector<double> counts;
  for (int i = 0; i < alphabet_size; i++)
    counts.push_back(static_cast<double>(generateRandomInteger(alphabet_size)));
  return tops::model::DiscreteIIDModel::make(counts);
}

}  // namespace helper
}  // namespace tops
