#include "Random.hpp"

#include <random>

namespace tops {
namespace helper {

std::default_random_engine generator;

int generateRandomInteger(int max) {
  std::uniform_int_distribution<int> distribution(0, max);
  return distribution(generator);
}

}  // namespace helper
}  // namespace tops