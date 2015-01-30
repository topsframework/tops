#include "Random.hpp"

#include <chrono>
#include <random>

namespace tops {
namespace helper {

std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());

int generateRandomDouble() {
  std::uniform_real_distribution<double> distribution(0.0, 1.0);
  return distribution(generator);
}

}  // namespace helper
}  // namespace tops