#include "DiscreteIIDModel.hpp"

#include "src/DiscreteIIDModel.hpp"

namespace tops {
  namespace model {
    DiscreteIIDModel::DiscreteIIDModel(const std::vector<double> &log_probabilities) {
      setTopsModel(tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(log_probabilities)));
    }

    double DiscreteIIDModel::choose() const {
      return getTopsModel()->choose();
    }

    Symbol DiscreteIIDModel::choosePosition(const Sequence &sequence, unsigned int index) const {
      return getTopsModel()->factorable()->choosePosition(sequence, index);
    }

    double DiscreteIIDModel::logProbabilityOf(Symbol symbol) {
      return getTopsModel()->log_probability_of(symbol);
    }

    double DiscreteIIDModel::evaluate(const Sequence &sequence, unsigned int begin, unsigned int end) {
      return getTopsModel()->evaluate(sequence, begin, end);
    }

    double DiscreteIIDModel::evaluatePosition(const Sequence &sequence, unsigned int index) const {
      return getTopsModel()->evaluatePosition(sequence, index);
    }
  }
}
