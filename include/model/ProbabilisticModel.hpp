#ifndef PROBABILISTIC_MODEL_HPP_
#define PROBABILISTIC_MODEL_HPP_

#include "Sequence.hpp"

#include "src/ProbabilisticModel.hpp"

namespace tops {
  namespace model {
    class ProbabilisticModel {
    public:
      // After refactoring, remove _self!
      tops::ProbabilisticModelPtr _self;
    };
  }
}

#endif
