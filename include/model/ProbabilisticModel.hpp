#ifndef PROBABILISTIC_MODEL_HPP_
#define PROBABILISTIC_MODEL_HPP_

#include "Sequence.hpp"

#include "src/ProbabilisticModel.hpp"

namespace tops {
  namespace model {
    class ProbabilisticModel {
    public:
      // After refactoring, remove topsModel methods!
      void setTopsModel(ProbabilisticModelPtr model) {
        _tops_model = model;
      }

      tops::ProbabilisticModelPtr getTopsModel() const {
        return _tops_model;
      }
    private:
      tops::ProbabilisticModelPtr _tops_model;
    };
  }
}

#endif
