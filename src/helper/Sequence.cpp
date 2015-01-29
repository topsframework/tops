#include "helper/Sequence.hpp"

#include "helper/Random.hpp"

namespace tops {
namespace helper {

tops::model::Sequence generateRandomSequence(int size, int alphabet_size) {
  tops::model::Sequence sequence;
  for (int i = 0; i < size; i++) {
    sequence.push_back(generateRandomInteger(alphabet_size-1));
  }
  return sequence;
}

}  // namespace helper
}  // namespace tops
