#include "Random.hpp"

#include <random>

namespace tops {
namespace helper {

std::default_random_engine generator;

int generateRandomInteger(int max) {
  std::uniform_int_distribution<int> distribution(0, max);
  return distribution(generator);
}

Sequence generateSequence(int size, int alphabet_size) {
  Sequence sequence;
  for (int i = 0; i < size; i++) {
    sequence.push_back(generateRandomInteger(alphabet_size-1));
  }
  return sequence;
}


}  // namespace helper
}  // namespace tops